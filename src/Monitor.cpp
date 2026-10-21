#include "Monitor.hpp"

#include "CpuCollector.hpp"
#include "MemoryCollector.hpp"
#include "ProcessCollector.hpp"

#include <chrono>
#include <unordered_map>

SystemSnapshot collect_snapshot()
{
    SystemSnapshot snapshot;

    // Take the timestamp right before reading /proc/stat so every snapshot
    // is timed at the same point in collection.
    snapshot.time = std::chrono::steady_clock::now();
    snapshot.cpu = CpuCollector{}.collect();
    snapshot.processes = ProcessCollector{}.collect();
    snapshot.memory = MemoryCollector{}.collect();

    return snapshot;
}

std::optional<double> calculate_overall_cpu(const CpuCounters& a, const CpuCounters& b)
{
    // Only these eight fields count toward the total for this milestone.
    const std::uint64_t total_a = a.user + a.nice + a.system + a.idle + a.iowait + a.irq + a.softirq + a.steal;
    const std::uint64_t total_b = b.user + b.nice + b.system + b.idle + b.iowait + b.irq + b.softirq + b.steal;

    // idle + iowait counts as non-busy time.
    const std::uint64_t idle_a = a.idle + a.iowait;
    const std::uint64_t idle_b = b.idle + b.iowait;

    // The counters are unsigned, so check they didn't go down before
    // subtracting (iowait in particular can decrease).
    if (total_b < total_a || idle_b < idle_a)
    {
        return std::nullopt;
    }

    const std::uint64_t delta_total = total_b - total_a;
    const std::uint64_t delta_idle = idle_b - idle_a;

    // No time passed, or the numbers don't make sense together.
    if (delta_total == 0 || delta_idle > delta_total)
    {
        return std::nullopt;
    }

    return 100.0 * static_cast<double>(delta_total - delta_idle) / static_cast<double>(delta_total);
}

void calculate_metrics(const SystemSnapshot& previous, SystemSnapshot& current, long ticks_per_second)
{
    current.cpu_usage = calculate_overall_cpu(previous.cpu, current.cpu);

    // Use the real time between snapshots, not the requested interval.
    const double elapsed = std::chrono::duration<double>(current.time - previous.time).count();
    if (elapsed <= 0.0 || ticks_per_second <= 0)
    {
        return; // every process keeps cpu_usage empty ("--")
    }

    // Look up previous records by PID so matching is fast.
    std::unordered_map<int, const ProcessInfo*> previousByPid;
    for (const ProcessInfo& p : previous.processes)
    {
        previousByPid[p.pid] = &p;
    }

    for (ProcessInfo& b : current.processes)
    {
        const auto found = previousByPid.find(b.pid);
        if (found == previousByPid.end())
        {
            continue; // new process: no earlier sample yet, so CPU stays "--"
        }

        const ProcessInfo& a = *found->second;

        // A different start time means the PID was reused by a new process,
        // so the old counters don't belong to this one.
        if (a.start_ticks != b.start_ticks || b.cpu_ticks < a.cpu_ticks)
        {
            continue;
        }

        const double cpu_seconds = static_cast<double>(b.cpu_ticks - a.cpu_ticks) /
                                   static_cast<double>(ticks_per_second);

        // 100% means one fully busy core, so multithreaded processes can go above 100%.
        b.cpu_usage = 100.0 * cpu_seconds / elapsed;
    }
}
