cmake_minimum_required(VERSION 3.20)
project(SystemObservabilityMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Collectors and reporter live in a library, separate from main.cpp.
# Later milestones add CpuCollector, filter/sort, etc. here.
add_library(monitor_core
    src/MemoryCollector.cpp
    src/ProcessCollector.cpp
    src/Reporter.cpp
)
target_include_directories(monitor_core PUBLIC include)
target_compile_options(monitor_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(system_observability_monitor src/main.cpp)
target_link_libraries(system_observability_monitor PRIVATE monitor_core)
target_compile_options(system_observability_monitor PRIVATE -Wall -Wextra -Wpedantic)
