#pragma once

#include "MemoryInfo.hpp"
#include "ProcessInfo.hpp"

#include <vector>

// Contains a view of the system at a single point in time.
struct SystemSnapshot {
    MemoryInfo memory;
    std::vector<ProcessInfo> processes;
};
