#include "Reporter.hpp"

#include <iomanip>
#include <sstream>

namespace
{
    // Formats a percentage with one decimal place, or "--" if unavailable.
    // Uses its own string stream so std::fixed doesn't stick to m_out.
    std::string formatPercent(const std::optional<double>& value)
    {
        if (!value)
        {
            return "--";
        }
        std::ostringstream text;
        text << std::fixed << std::setprecision(1) << *value;
        return text.str();
    }

    // Formats a kB value, or "--" if unavailable.
    std::string formatKb(const std::optional<std::uint64_t>& value)
    {
        if (!value)
        {
            return "--";
        }
        return std::to_string(*value);
    }
} // namespace

Reporter::Reporter(std::ostream& out) :
    m_out(out)
{
}

void Reporter::print(const SystemSnapshot& snapshot,
                     const std::vector<ProcessInfo>& rows,
                     const std::string& sortLabel,
                     long long intervalMs) const
{
    const MemoryInfo& mem = snapshot.memory;

    std::string cpuText = formatPercent(snapshot.cpu_usage);
    if (snapshot.cpu_usage)
    {
        cpuText += "%";
    }

    // Summary lines. "Processes collected" is the full count, not just the
    // rows shown below.
    m_out << "CPU: " << cpuText
          << "   Memory (kB): total=" << mem.total_kb
          << " used=" << mem.used_kb
          << " available=" << mem.available_kb << '\n';
    m_out << "Processes collected: " << snapshot.processes.size()
          << " | Showing: " << rows.size()
          << " | Sort: " << sortLabel
          << " | Interval: " << intervalMs << " ms\n\n";

    // Process table.
    m_out << std::right << std::setw(7) << "PID" << "  "
          << std::left << std::setw(16) << "NAME"
          << std::right << std::setw(8) << "CPU%"
          << std::setw(12) << "RSS (kB)" << '\n';

    for (const ProcessInfo& p : rows)
    {
        // Cut long names (some kernel threads) so the columns stay lined up.
        m_out << std::right << std::setw(7) << p.pid << "  "
              << std::left << std::setw(16) << p.name.substr(0, 15)
              << std::right << std::setw(8) << formatPercent(p.cpu_usage)
              << std::setw(12) << formatKb(p.memory_kb) << '\n';
    }
}
