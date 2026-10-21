# System Observability Monitor

CS3460 - Fall 2026

Brooklyn Gull (Team Leader)

* **A-Number:** A02392549

* **USU Email:** A02392549@usu.edu
---

## Build and Run Instructions

1. Build the project:
```bash
   ./build.sh
```
   If you get `Permission denied`, give the script execution permissions first:
```bash
   chmod +x build.sh
```
2. Run the monitor:
```bash
   ./build/system_observability_monitor
```
   It refreshes until you press **Ctrl+C**.

---

## Command-Line Options

| Option | Description | Default |
|---|---|---|
| `-i`, `--interval <ms>` | Time between reports in milliseconds | `1000` |
| `-n`, `--top <N>` | Number of processes to show | `10` |
| `-s`, `--sort <cpu\|memory>` | Sort processes by CPU or memory (highest first) | `cpu` |
| `-c`, `--count <N>` | Stop after N reports (`0` = run until Ctrl+C) | `0` |
| `-h`, `--help` | Show usage information | |

Examples:

```bash
./build/system_observability_monitor -i 2000             # refresh every 2 seconds
./build/system_observability_monitor -s memory -n 5      # top 5 processes by memory
./build/system_observability_monitor -i 500 -n 20 -c 10  # 10 reports, 500 ms apart
```

---

## Example Output

```
Collecting baseline sample... first report in 1000 ms

===== Report 1 =====
CPU: 50.2%   Memory (kB): total=8222320 used=571556 available=7650764
Processes collected: 68 | Showing: 4 | Sort: CPU | Interval: 1000 ms

    PID  NAME                CPU%    RSS (kB)
   2989  sh                  98.7        1852
   1476  claude               2.0      329016
      1  process_api          0.0        5820
      2  kthreadd             0.0          --
```

Notes:

* `--` means the value is unavailable, not zero. Kernel threads have no resident memory, and a process that just started has no CPU % until the next report.
* Process CPU % is measured per core, so a multithreaded process can go above 100%.

---

## Project Structure

```
include/   Header files (data types and class declarations)
src/       Source files
  CpuCollector       Reads overall CPU counters from /proc/stat
  MemoryCollector    Reads memory totals from /proc/meminfo
  ProcessCollector   Reads each process's name, CPU time, and memory from /proc/<pid>/
  Monitor            Collects snapshots and calculates CPU %
  ProcessSorter      Sorts processes and keeps the top N
  Reporter           Formats and prints the report
  main               Command-line options and the refresh loop
build.sh   Builds the project with Clang and CMake
```
