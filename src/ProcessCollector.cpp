// ProcessCollector.cpp
//
// Builds a list of running processes by scanning /proc. Each running process
// has a directory named after its PID (e.g. /proc/1234) containing:
//   comm   - the process name
//   stat   - CPU time counters (utime, stime) and the start time
//   status - human-readable info, including VmRSS (resident memory)

#include "ProcessCollector.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

ProcessCollector::ProcessCollector(std::filesystem::path procRoot) :
    m_root(std::move(procRoot))
{
}

// Returns true if the name is non-empty and every character is a digit.
// This separates process directories ("1", "4021") from other /proc entries
// such as "self", "sys", and "net".
bool ProcessCollector::isNumeric(std::string_view name)
{
    return !name.empty() &&
           std::all_of(name.begin(), name.end(),
                       [](unsigned char c) { return std::isdigit(c); });
}

// Parses one line of /proc/<pid>/stat and fills in cpu_ticks and start_ticks.
// Field 14 is utime, field 15 is stime, and field 22 is starttime.
// Returns false if the line doesn't have the fields we need.
bool ProcessCollector::readProcessCpu(const std::string& line, ProcessInfo& p)
{
    // Field 2 is the name in parentheses, and the name itself can contain
    // spaces or ")" characters. Start parsing after the LAST ")".
    const std::size_t close = line.rfind(')');
    if (close == std::string::npos)
    {
        return false;
    }

    std::istringstream fields(line.substr(close + 1));
    std::string ignored;

    // Skip fields 3 through 13.
    for (int field = 3; field <= 13; ++field)
    {
        if (!(fields >> ignored))
        {
            return false;
        }
    }

    std::uint64_t utime{};
    std::uint64_t stime{};
    if (!(fields >> utime >> stime)) // fields 14 and 15
    {
        return false;
    }

    // Skip fields 16 through 21.
    for (int field = 16; field <= 21; ++field)
    {
        if (!(fields >> ignored))
        {
            return false;
        }
    }

    if (!(fields >> p.start_ticks)) // field 22
    {
        return false;
    }

    p.cpu_ticks = utime + stime;
    return true;
}

// Looks for the "VmRSS:" line in /proc/<pid>/status and returns its value in kB.
// Kernel threads have no VmRSS line, so this returns an empty optional for them.
std::optional<std::uint64_t> ProcessCollector::readResidentMemory(const std::filesystem::path& statusPath)
{
    std::ifstream status(statusPath);
    std::string line;

    while (std::getline(status, line))
    {
        std::istringstream row(line);
        std::string key;
        std::uint64_t value{};
        if (row >> key >> value && key == "VmRSS:")
        {
            return value;
        }
    }

    return std::nullopt;
}

// Scans /proc and returns every process we could read, sorted by PID.
std::vector<ProcessInfo> ProcessCollector::collect() const
{
    std::vector<ProcessInfo> processes;

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(m_root))
        {
            const std::string dirName = entry.path().filename().string();
            if (!isNumeric(dirName) || !entry.is_directory())
            {
                continue;
            }

            ProcessInfo process;
            process.pid = std::stoi(dirName);

            // The process may have exited since we listed the directory.
            // If comm or stat can't be read, skip it and move on.
            std::ifstream comm(entry.path() / "comm");
            if (!std::getline(comm, process.name))
            {
                continue;
            }

            std::ifstream stat(entry.path() / "stat");
            std::string statLine;
            if (!std::getline(stat, statLine) || !readProcessCpu(statLine, process))
            {
                continue;
            }

            // Missing VmRSS is normal (e.g. kernel threads). Keep the process
            // and leave memory_kb empty so the reporter prints "--".
            process.memory_kb = readResidentMemory(entry.path() / "status");

            processes.push_back(process);
        }
    }
    catch (const std::filesystem::filesystem_error&)
    {
        // /proc changed while we were reading it. Keep what we collected.
    }

    // directory_iterator gives no particular order, so sort by PID.
    std::sort(processes.begin(), processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) { return a.pid < b.pid; });

    return processes;
}
