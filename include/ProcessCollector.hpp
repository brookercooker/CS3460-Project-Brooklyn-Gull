#pragma once

#include "ProcessInfo.hpp"

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

class ProcessCollector {
  public:
    explicit ProcessCollector(std::filesystem::path procRoot = "/proc");

    // Lists the numeric directories under procRoot and reads the comm file for each one.
    // Skips processes that exit during the scan. Result is sorted by PID.
    [[nodiscard]] std::vector<ProcessInfo> collect() const;

    // Only true for a non-empty string made entirely of digits.
    [[nodiscard]] static bool isNumeric(std::string_view name);

  private:
    [[nodiscard]] static std::optional<ProcessInfo> readProcess(const std::filesystem::path& dir, int pid);

    std::filesystem::path m_root;
};
