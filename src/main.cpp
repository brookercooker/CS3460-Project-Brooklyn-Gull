#include "MemoryCollector.hpp"
#include "ProcessCollector.hpp"
#include "Reporter.hpp"
#include "SystemSnapshot.hpp"

#include <exception>
#include <iostream>

int main() {
    // Takes a snapshot and reports, computes used memory with Memory Collector and enumberates/reads with Process Collector
    try {
        SystemSnapshot snapshot;
        snapshot.memory = MemoryCollector{}.collect();
        snapshot.processes = ProcessCollector{}.collect();

        Reporter{ std::cout }.print(snapshot);
    }
    // Reports any errors, else returns 0
    catch (const std::exception& e) {
        std::cerr << "system_observability_monitor: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
