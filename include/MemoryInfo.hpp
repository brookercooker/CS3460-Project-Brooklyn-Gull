#pragma once

#include <cstdint>

// Values in kB as reported by proc/meminfo.
struct MemoryInfo {
    std::uint64_t total_kb{};
    std::uint64_t available_kb{};
    std::uint64_t used_kb{};
};
