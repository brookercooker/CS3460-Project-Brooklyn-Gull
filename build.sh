#!/usr/bin/env bash
# Builds the entire project with Clang and CMake.
# Usage: ./build.sh
set -euo pipefail

# Always run from the repository root, even if called from elsewhere.
cd "$(dirname "$0")"

if ! command -v clang++ >/dev/null 2>&1; then
    echo "error: clang++ not found. Install Clang (e.g. sudo apt install clang)." >&2
    exit 1
fi

cmake -S . -B build \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j

echo
echo "Build succeeded. Run with: ./build/system_observability_monitor"
