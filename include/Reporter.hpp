#pragma once

#include "ProcessInfo.hpp"
#include "SystemSnapshot.hpp"

#include <ostream>
#include <string>
#include <vector>

// Formats completed data. Never reads /proc or calculates anything.
class Reporter
{
  public:
    explicit Reporter(std::ostream& out);

    // snapshot - the full snapshot (used for overall CPU, memory and the
    //            total process count)
    // rows     - the already sorted and trimmed display copy
    void print(const SystemSnapshot& snapshot,
               const std::vector<ProcessInfo>& rows,
               const std::string& sortLabel,
               long long intervalMs) const;

  private:
    std::ostream& m_out;
};
