#pragma once

#include <string>

// Contains information collected on processes.
struct ProcessInfo {
    int pid{};
    std::string name;
};
