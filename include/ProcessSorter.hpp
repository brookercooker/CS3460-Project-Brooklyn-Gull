#pragma once

#include "ProcessInfo.hpp"

#include <cstddef>
#include <vector>

enum class SortKey
{
    Cpu,
    Memory
};

// Comparators for std::sort: highest value first, unavailable values last,
// and smaller PID first when values tie.
[[nodiscard]] bool compareByCpu(const ProcessInfo& a, const ProcessInfo& b);
[[nodiscard]] bool compareByMemory(const ProcessInfo& a, const ProcessInfo& b);

// Sorts rows by the chosen key, then keeps only the first top_n.
// Call this on a display copy, never on the snapshot's own process list.
void sort_and_limit(std::vector<ProcessInfo>& rows, SortKey sort_by, std::size_t top_n);
