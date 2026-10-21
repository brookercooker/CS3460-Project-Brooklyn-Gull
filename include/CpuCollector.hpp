#pragma once

#include "CpuCounters.hpp"

#include <filesystem>

// Reads the overall CPU counters from /proc/stat.
// It only reads raw counters; CPU% is calculated later from two snapshots.
class CpuCollector
{
  public:
    explicit CpuCollector(std::filesystem::path statPath = "/proc/stat");

    // Throws std::runtime_error if the file can't be read or the first
    // line isn't a valid "cpu" line.
    [[nodiscard]] CpuCounters collect() const;

  private:
    std::filesystem::path m_path;
};
