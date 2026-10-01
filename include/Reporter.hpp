#pragma once

#include "SystemSnapshot.hpp"

#include <ostream>

// Formats a completed snapshot
class Reporter {
  public:
    explicit Reporter(std::ostream& out);

    void print(const SystemSnapshot& snapshot) const;

  private:
    std::ostream& m_out;
};
