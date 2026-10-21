#pragma once

#include <cstdint>

// The first eight cumulative counters from the "cpu" line of /proc/stat.
// Each one counts clock ticks spent in that state since boot.
struct CpuCounters
{
    std::uint64_t user{}, nice{}, system{}, idle{};
    std::uint64_t iowait{}, irq{}, softirq{}, steal{};
};
