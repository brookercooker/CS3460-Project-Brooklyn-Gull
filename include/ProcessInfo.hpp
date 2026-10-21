#pragma once

#include <cstdint>
#include <optional>
#include <string>

// One process. An empty optional means "unavailable", not zero.
struct ProcessInfo
{
    int pid{};                                // Kept from M1
    std::string name;                         // Kept from M1
    std::uint64_t cpu_ticks{}, start_ticks{}; // Raw counters from /proc/<pid>/stat
    std::optional<double> cpu_usage;          // CPU%, filled in by calculate_metrics
    std::optional<std::uint64_t> memory_kb;   // VmRSS from /proc/<pid>/status
};
