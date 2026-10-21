#pragma once

#include "ProcessInfo.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class ProcessCollector
{
  public:
    explicit ProcessCollector(std::filesystem::path procRoot = "/proc");

    // Lists numeric directories under procRoot and reads each process's name,
    // CPU counters and resident memory. Processes that exit mid-scan are
    // skipped. Result is sorted by PID. cpu_usage is left empty here; it is
    // filled in later by calculate_metrics.
    [[nodiscard]] std::vector<ProcessInfo> collect() const;

    // True only for a non-empty string made entirely of decimal digits.
    [[nodiscard]] static bool isNumeric(std::string_view name);

    // Parses one /proc/<pid>/stat line into p.cpu_ticks and p.start_ticks.
    [[nodiscard]] static bool readProcessCpu(const std::string& line, ProcessInfo& p);

  private:
    // Returns VmRSS in kB from a /proc/<pid>/status file, if present.
    [[nodiscard]] static std::optional<std::uint64_t> readResidentMemory(const std::filesystem::path& statusPath);

    std::filesystem::path m_root;
};
