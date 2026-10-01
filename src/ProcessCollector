#include "ProcessCollector.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <string>
#include <system_error>

ProcessCollector::ProcessCollector(std::filesystem::path procRoot) :
    m_root(std::move(procRoot)) {
}

// Confirms that every character is a digit
bool ProcessCollector::isNumeric(std::string_view name) {
    return !name.empty() &&
           std::all_of(name.begin(), name.end(),
                       [](unsigned char c) { return std::isdigit(c) != 0; });
}

// Reads the name of a single process
std::optional<ProcessInfo> ProcessCollector::readProcess(const std::filesystem::path& dir, int pid) {
    std::ifstream comm(dir / "comm");
    std::string name;
    if (!comm || !std::getline(comm, name)) {
        return std::nullopt; // If process exited or is unreadable
    }
    return ProcessInfo{ pid, std::move(name) };
}

// Scans /proc and returns every readable process, sorted by PID.
// Missing/vanished processes are skipped.
std::vector<ProcessInfo> ProcessCollector::collect() const {
    std::vector<ProcessInfo> processes;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_root)) {
            const std::string dirName = entry.path().filename().string();
            if (!isNumeric(dirName) || !entry.is_directory()) {
                continue;
            }

            // IF comm can't be read (process may have already exited) then continue
            std::ifstream comm(entry.path() / "comm");
            std::string processName;
            if (!std::getline(comm, processName)) {
                continue;
            }

            processes.push_back(ProcessInfo{ std::stoi(dirName), processName });
        }
    }
    catch (const std::filesystem::filesystem_error&)
    {
        // /proc updated while being read, preserve collected information
    }

    // Sort by PID
    std::sort(processes.begin(), processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) { return a.pid < b.pid; });

    return processes;
}
