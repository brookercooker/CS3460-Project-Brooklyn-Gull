#include "ProcessSorter.hpp"

#include <algorithm>

bool compareByCpu(const ProcessInfo& a, const ProcessInfo& b)
{
    // Unavailable CPU becomes -1 so it sorts after every real value (>= 0).
    const double cpu_a = a.cpu_usage.value_or(-1.0);
    const double cpu_b = b.cpu_usage.value_or(-1.0);

    if (cpu_a == cpu_b)
    {
        return a.pid < b.pid; // Smaller PID first when CPU values tie.
    }
    return cpu_a > cpu_b; // Higher CPU usage first.
}

bool compareByMemory(const ProcessInfo& a, const ProcessInfo& b)
{
    // Can't use -1 for an unsigned value, so check availability directly.
    const bool has_a = a.memory_kb.has_value();
    const bool has_b = b.memory_kb.has_value();

    if (has_a != has_b)
    {
        return has_a; // The one with a value comes first.
    }
    if (!has_a || *a.memory_kb == *b.memory_kb)
    {
        return a.pid < b.pid; // Both missing, or a tie: smaller PID first.
    }
    return *a.memory_kb > *b.memory_kb; // More memory first.
}

void sort_and_limit(std::vector<ProcessInfo>& rows, SortKey sort_by, std::size_t top_n)
{
    if (sort_by == SortKey::Cpu)
    {
        std::sort(rows.begin(), rows.end(), compareByCpu);
    }
    else
    {
        std::sort(rows.begin(), rows.end(), compareByMemory);
    }

    // Sort first, then keep the top N.
    if (rows.size() > top_n)
    {
        rows.resize(top_n);
    }
}
