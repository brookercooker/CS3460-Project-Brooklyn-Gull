#include "Reporter.hpp"

#include <iomanip>

namespace {
    void printKb(std::ostream& out, std::uint64_t kb)
    {
        const double mib = static_cast<double>(kb) / 1024.0;
        out << std::setw(12) << kb << " kB  ("
            << std::fixed << std::setprecision(1) << std::setw(9) << mib << " MiB)";
    }
}

Reporter::Reporter(std::ostream& out) :
    m_out(out) {
}

void Reporter::print(const SystemSnapshot& snapshot) const {
    const MemoryInfo& mem = snapshot.memory;
    const double usedPct = mem.total_kb == 0
                               ? 0.0
                               : 100.0 * static_cast<double>(mem.used_kb) / static_cast<double>(mem.total_kb);

    // Save stream formatting so std::fixed and other items aren't leaked to the caller.
    const std::ios_base::fmtflags oldFlags = m_out.flags();
    const std::streamsize oldPrecision = m_out.precision();

    // Print a formatted output of the system snapshot.
    m_out << "=== System Snapshot ===\n";
    m_out << "Memory\n";
    m_out << "  Total:     ";
    printKb(m_out, mem.total_kb);
    m_out << '\n';
    m_out << "  Used:      ";
    printKb(m_out, mem.used_kb);
    m_out << "  " << std::setw(5) << usedPct << "%\n";
    m_out << "  Available: ";
    printKb(m_out, mem.available_kb);
    m_out << "\n\n";

    m_out << "Processes: " << snapshot.processes.size() << '\n';
    m_out << "  " << std::setw(7) << "PID" << "  NAME\n";
    for (const auto& p : snapshot.processes) {
        m_out << "  " << std::setw(7) << p.pid << "  " << p.name << '\n';
    }

    m_out.flags(oldFlags);
    m_out.precision(oldPrecision);
}
