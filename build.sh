#!/bin/bash

# Exit immediately if any command fails
set -e

# Define build directory
BUILD_DIR="build"

echo "Creating build directory..."
mkdir -p $BUILD_DIR

echo "Configuring project with CMake using Clang..."
# Explicitly sets Clang as the compiler for C++20 compliance
cmake -S . -B $BUILD_DIR \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_BUILD_TYPE=Release

echo "Building project..."
cmake --build $BUILD_DIR --config Release

echo "Build complete. Executable located in the '$BUILD_DIR' directory."
