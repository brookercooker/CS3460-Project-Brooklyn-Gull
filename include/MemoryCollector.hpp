#pragma once

#include "MemoryInfo.hpp"

#include <filesystem>
#include <istream>

class MemoryCollector {
  public:
    explicit MemoryCollector(std::filesystem::path meminfoPath = "/proc/meminfo");

    // Gets info in meminfo file.
    [[nodiscard]] MemoryInfo collect() const;

    // Parses the input taken from memInfo. 
    [[nodiscard]] static MemoryInfo parse(std::istream& in);

  private:
    std::filesystem::path m_path;
};
