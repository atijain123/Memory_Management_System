# Memory Management Simulator

A C++17 command-line simulator for basic operating-system memory management concepts. It models contiguous RAM allocation, external fragmentation, block coalescing, and a two-level CPU cache hierarchy.

## Features

- Contiguous memory allocation over a simulated byte-addressable RAM space.
- Runtime allocator selection:
  - First Fit
  - Best Fit
  - Worst Fit
- Free block coalescing after successful deallocation.
- Memory layout dumping with hexadecimal address ranges.
- Memory statistics:
  - total memory
  - used memory
  - free memory
  - utilization
  - external fragmentation
  - largest free block
- L1/L2 cache simulation:
  - L1: 1024 bytes, 2-way set associative, 64-byte blocks
  - L2: 4096 bytes, 4-way set associative, 64-byte blocks
  - FIFO replacement using per-set insertion queues
  - hit, miss, and hit-rate statistics

## Build

With `make`:

```bash
make
```

Without `make`:

```bash
mkdir -p bin
g++ -std=c++17 -Wall -Wextra -g -Iinclude \
  src/main.cpp src/SimulatorShell.cpp \
  src/allocator/MemoryManager.cpp src/allocator/FirstFit.cpp \
  src/allocator/BestFit.cpp src/allocator/WorstFit.cpp \
  src/cache/cache.cpp \
  -o bin/memsim
```

On some Windows bash setups, you may prefer:

```bash
g++ -std=c++17 -Wall -Wextra -g -Iinclude \
  src/main.cpp src/SimulatorShell.cpp \
  src/allocator/MemoryManager.cpp src/allocator/FirstFit.cpp \
  src/allocator/BestFit.cpp src/allocator/WorstFit.cpp \
  src/cache/cache.cpp \
  -o bin/memsim.exe
```

## Run

```bash
./bin/memsim
```

or, if you built the `.exe`:

```bash
./bin/memsim.exe
```

Example session:

```text
init memory 1024
set allocator first fit
malloc 100
malloc 200
dump memory
free 0
stats
read 64
read 65
exit
```

## Commands

| Command | Purpose |
| --- | --- |
| `init memory <size>` | Create/reset the simulated physical memory |
| `set allocator <strategy>` | Use `first fit`, `best fit`, or `worst fit` |
| `malloc <size>` | Allocate a block of memory |
| `alloc <size>` | Alias for `malloc` |
| `free <address>` | Free a block by its starting address |
| `dump memory` | Print the current memory block layout |
| `read <address>` | Simulate a cache-backed memory read |
| `write <address>` | Simulate a cache-backed memory write |
| `access <address>` | Generic alias for cache access |
| `reset cache` | Clear cache lines and cache statistics |
| `stats` | Print memory and cache statistics |
| `help` | Show supported commands |
| `exit` | Quit the simulator |

Allocator names also accept short forms:

```text
set allocator first
set allocator best
set allocator worst
```

## Tests

Run all included test scripts:

```bash
./run_tests.sh
```

Run individual command files:

```bash
./bin/memsim < tests/fragmentation_test.txt
./bin/memsim < tests/cache_test.txt
./bin/memsim < tests/strategy_comparison_test.txt
```

Use `.exe` in the command if that is the binary you built.
