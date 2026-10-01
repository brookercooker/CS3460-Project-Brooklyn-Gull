#include "MemoryCollector.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

MemoryCollector::MemoryCollector(std::filesystem::path meminfoPath) :
    m_path(std::move(meminfoPath)) {
}

MemoryInfo MemoryCollector::collect() const {
    // Throw error if file can't be read
    std::ifstream in(m_path);
    if (!in) {
        throw std::runtime_error("cannot open " + m_path.string());
    }
    return parse(in);
}

MemoryInfo MemoryCollector::parse(std::istream& in) {
    MemoryInfo info;
    bool haveTotal = false;
    bool haveAvailable = false;

    // Read through file line by line to get MemTotal and MemAvailable
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream row(line);
        std::string key;
        std::uint64_t value = 0;
        if (!(row >> key >> value)) {
            continue;
        }

        if (key == "MemTotal:") {
            info.total_kb = value;
            haveTotal = true;
        }
        else if (key == "MemAvailable:") {
            info.available_kb = value;
            haveAvailable = true;
        }

        if (haveTotal && haveAvailable) {
            break;
        }
    }

    // Throw error if data is missing
    if (!haveTotal || !haveAvailable) {
        throw std::runtime_error("meminfo is missing MemTotal or MemAvailable");
    }

    // Prevent unsigned overflow so that used doesn't exceed the total.
    if (info.available_kb > info.total_kb) {
        info.available_kb = info.total_kb;
    }
    info.used_kb = info.total_kb - info.available_kb;
    return info;
}
