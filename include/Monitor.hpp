#pragma once

#include "SystemSnapshot.hpp"

#include <optional>

// Reads memory, CPU counters and all processes, and timestamps the result.
// CPU percentages are left empty; calculate_metrics fills them in.
[[nodiscard]] SystemSnapshot collect_snapshot();

// Overall CPU% from two sets of /proc/stat counters.
// Returns an empty optional if no valid percentage can be calculated.
[[nodiscard]] std::optional<double> calculate_overall_cpu(const CpuCounters& previous, const CpuCounters& current);

// Fills in current.cpu_usage and each current.processes[i].cpu_usage by
// comparing against the previous snapshot. ticks_per_second comes from
// sysconf(_SC_CLK_TCK).
void calculate_metrics(const SystemSnapshot& previous, SystemSnapshot& current, long ticks_per_second);
