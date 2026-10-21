#pragma once

#include "CpuCounters.hpp"
#include "MemoryInfo.hpp"
#include "ProcessInfo.hpp"

#include <chrono>
#include <optional>
#include <vector>

// One point-in-time view of the system.
struct SystemSnapshot
{
    MemoryInfo memory;                          // Kept from M1
    std::vector<ProcessInfo> processes;         // Kept from M1, now with CPU/memory fields
    CpuCounters cpu;                            // Raw counters from /proc/stat
    std::chrono::steady_clock::time_point time; // When this snapshot was taken
    std::optional<double> cpu_usage;            // Overall CPU%, filled in by calculate_metrics
};
