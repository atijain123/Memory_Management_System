#!/bin/bash

set -e

if command -v make >/dev/null 2>&1; then
    make
else
    mkdir -p bin
    g++ -std=c++17 -Wall -Wextra -g -Iinclude \
        src/main.cpp src/SimulatorShell.cpp \
        src/allocator/MemoryManager.cpp src/allocator/FirstFit.cpp \
        src/allocator/BestFit.cpp src/allocator/WorstFit.cpp \
        src/cache/cache.cpp \
        -o bin/memsim
fi

SIM="./bin/memsim"
if [ -x "./bin/memsim.exe" ]; then
    SIM="./bin/memsim.exe"
fi

echo "--------------------------------------"
echo "Running Fragmentation Test..."
echo "--------------------------------------"
"$SIM" < tests/fragmentation_test.txt

echo ""
echo "--------------------------------------"
echo "Running Cache Locality Test..."
echo "--------------------------------------"
"$SIM" < tests/cache_test.txt

echo ""
echo "--------------------------------------"
echo "Running Strategy Comparison Test..."
echo "--------------------------------------"
"$SIM" < tests/strategy_comparison_test.txt
