#include "CpuCollector.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

CpuCollector::CpuCollector(std::filesystem::path statPath) :
    m_path(std::move(statPath))
{
}

CpuCounters CpuCollector::collect() const
{
    std::ifstream input(m_path);
    std::string line;
    std::string label;
    CpuCounters counters{};

    if (!std::getline(input, line))
    {
        throw std::runtime_error("Cannot read /proc/stat");
    }

    // The first line looks like: "cpu  4705 356 584 3699 23 23 0 0 0 0"
    // We only use the first eight numbers.
    std::istringstream row(line);
    if (!(row >> label >> counters.user >> counters.nice
              >> counters.system >> counters.idle >> counters.iowait
              >> counters.irq >> counters.softirq >> counters.steal) ||
        label != "cpu")
    {
        throw std::runtime_error("Invalid aggregate CPU counters");
    }

    return counters;
}
