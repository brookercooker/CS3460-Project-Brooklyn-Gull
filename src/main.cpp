#include "Monitor.hpp"
#include "ProcessSorter.hpp"
#include "Reporter.hpp"
#include "SystemSnapshot.hpp"

#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace
{
    // Settings from the command line, with their defaults.
    struct Options
    {
        long long intervalMs = 1000; // time between reports
        std::size_t topN = 10;       // how many process rows to show
        SortKey sortBy = SortKey::Cpu;
        long long count = 0;         // number of reports; 0 means run until Ctrl+C
        bool showHelp = false;
    };

    void printUsage()
    {
        std::cout << "Usage: system_observability_monitor [options]\n"
                     "  -i, --interval <ms>     time between reports in milliseconds (default 1000)\n"
                     "  -n, --top <N>           number of processes to show (default 10)\n"
                     "  -s, --sort <cpu|memory> sort order (default cpu)\n"
                     "  -c, --count <N>         stop after N reports (default 0 = run until Ctrl+C)\n"
                     "  -h, --help              show this message\n";
    }

    // Converts an option's value to a number, or throws with a clear message.
    long long parseNumber(const std::string& option, const std::string& text)
    {
        try
        {
            std::size_t used = 0;
            const long long value = std::stoll(text, &used);
            if (used != text.size())
            {
                throw std::invalid_argument("trailing characters");
            }
            return value;
        }
        catch (const std::exception&)
        {
            throw std::runtime_error(option + " needs a whole number, got \"" + text + "\"");
        }
    }

    Options parseArgs(int argc, char* argv[])
    {
        Options options;

        for (int i = 1; i < argc; ++i)
        {
            const std::string arg = argv[i];

            if (arg == "-h" || arg == "--help")
            {
                options.showHelp = true;
                continue;
            }

            // Every other option needs a value after it.
            if (i + 1 >= argc)
            {
                throw std::runtime_error("missing value after " + arg);
            }
            const std::string value = argv[++i];

            if (arg == "-i" || arg == "--interval")
            {
                options.intervalMs = parseNumber(arg, value);
                if (options.intervalMs <= 0)
                {
                    throw std::runtime_error("interval must be greater than 0");
                }
            }
            else if (arg == "-n" || arg == "--top")
            {
                const long long top = parseNumber(arg, value);
                if (top <= 0)
                {
                    throw std::runtime_error("top must be greater than 0");
                }
                options.topN = static_cast<std::size_t>(top);
            }
            else if (arg == "-s" || arg == "--sort")
            {
                if (value == "cpu")
                {
                    options.sortBy = SortKey::Cpu;
                }
                else if (value == "memory" || value == "mem")
                {
                    options.sortBy = SortKey::Memory;
                }
                else
                {
                    throw std::runtime_error("sort must be \"cpu\" or \"memory\"");
                }
            }
            else if (arg == "-c" || arg == "--count")
            {
                options.count = parseNumber(arg, value);
                if (options.count < 0)
                {
                    throw std::runtime_error("count can't be negative");
                }
            }
            else
            {
                throw std::runtime_error("unknown option " + arg);
            }
        }

        return options;
    }
} // namespace

int main(int argc, char* argv[])
{
    try
    {
        const Options options = parseArgs(argc, argv);
        if (options.showHelp)
        {
            printUsage();
            return 0;
        }

        // Clock ticks per second, used to turn process CPU ticks into seconds.
        // Read it once here instead of every loop.
        const long ticksPerSecond = ::sysconf(_SC_CLK_TCK);
        if (ticksPerSecond <= 0)
        {
            throw std::runtime_error("could not read clock ticks per second");
        }

        const std::chrono::milliseconds interval(options.intervalMs);
        const std::string sortLabel = options.sortBy == SortKey::Cpu ? "CPU" : "Memory";
        const Reporter reporter(std::cout);

        // CPU% needs two samples, so take a baseline first.
        std::cout << "Collecting baseline sample... first report in "
                  << options.intervalMs << " ms\n"
                  << std::flush;
        SystemSnapshot previous = collect_snapshot();

        for (long long report = 1; options.count == 0 || report <= options.count; ++report)
        {
            std::this_thread::sleep_for(interval);

            SystemSnapshot current = collect_snapshot();
            calculate_metrics(previous, current, ticksPerSecond);

            // Sort and trim a copy so the full list stays as the next baseline.
            std::vector<ProcessInfo> rows = current.processes;
            sort_and_limit(rows, options.sortBy, options.topN);

            std::cout << "\n===== Report " << report << " =====\n";
            reporter.print(current, rows, sortLabel, options.intervalMs);
            std::cout << std::flush;

            previous = std::move(current);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "system_observability_monitor: " << e.what() << '\n';
        std::cerr << "Run with --help to see the options.\n";
        return 1;
    }
    return 0;
}
