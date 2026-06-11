# Memory Management Simulator

A command-line memory management simulator written in C++17. The project demonstrates how an operating system can manage contiguous physical memory and how a simple two-level CPU cache hierarchy responds to memory accesses.

The simulator is intended for learning and demonstration. It does not allocate real operating-system memory for user programs; instead, it models memory blocks, allocation strategies, fragmentation, cache hits, and cache misses inside a controlled simulation.

![Memory simulator output](Simulation.png)

## Project Goals

- Simulate contiguous physical memory allocation.
- Compare First Fit, Best Fit, and Worst Fit allocation strategies.
- Track used and free memory blocks through explicit metadata.
- Merge adjacent free blocks after deallocation.
- Calculate memory utilization and external fragmentation.
- Simulate L1 and L2 cache accesses with FIFO replacement.
- Provide a small interactive shell for running memory and cache commands.

## Core Concepts Demonstrated

### Memory Allocation

The simulator represents RAM as a byte-addressable memory space. Each allocation request searches for a suitable free block using the currently selected allocation strategy. If the selected block is larger than the requested size, it is split into:

- one used block
- one remaining free block

### Memory Deallocation

The `free <address>` command releases the block starting at the given address. After a successful free operation, adjacent free blocks are merged together. This process is called coalescing and helps reduce external fragmentation.

### External Fragmentation

External fragmentation occurs when free memory exists, but it is split into smaller separated regions. The simulator reports external fragmentation using the relationship between total free memory and the largest available free block.

### Cache Simulation

The simulator also models a small cache hierarchy:

| Cache | Size | Associativity | Block Size | Replacement |
| --- | ---: | ---: | ---: | --- |
| L1 | 1024 bytes | 2-way | 64 bytes | FIFO |
| L2 | 4096 bytes | 4-way | 64 bytes | FIFO |

When a memory address is accessed, the simulator checks:

1. L1 cache
2. L2 cache
3. Physical memory

The cache stores tags and valid bits for simulation purposes. It does not store actual data values.

## Features

- Interactive command-line shell.
- First Fit, Best Fit, and Worst Fit allocation.
- Short allocator aliases such as `first`, `best`, and `worst`.
- `malloc` and `alloc` commands for allocation.
- Block splitting during allocation.
- Block coalescing during deallocation.
- Memory dump with hexadecimal address ranges.
- Memory statistics for utilization and fragmentation.
- L1/L2 cache hit and miss simulation.
- Cache reset command.
- Scripted test inputs for repeatable demonstrations.

## Folder Structure

```text
memory-simulator/
├── docs/
│   └── DesignDocument.md
├── include/
│   ├── AllocatorStrategy.h
│   ├── BestFit.h
│   ├── Cache.h
│   ├── FirstFit.h
│   ├── MemoryManager.h
│   ├── SimulatorShell.h
│   └── WorstFit.h
├── src/
│   ├── main.cpp
│   ├── SimulatorShell.cpp
│   ├── allocator/
│   │   ├── BestFit.cpp
│   │   ├── FirstFit.cpp
│   │   ├── MemoryManager.cpp
│   │   └── WorstFit.cpp
│   └── cache/
│       └── cache.cpp
├── tests/
│   ├── cache_test.txt
│   ├── fragmentation_test.txt
│   └── strategy_comparison_test.txt
├── Makefile
├── README.md
└── run_tests.sh
```

## Architecture

The project is separated into four main parts.

| Component | Responsibility |
| --- | --- |
| `SimulatorShell` | Reads commands, validates input, and calls the simulator components |
| `MemoryManager` | Maintains simulated RAM, block metadata, allocation, free, coalescing, and memory stats |
| `AllocatorStrategy` | Defines the interface used by allocation algorithms |
| `Cache` | Simulates set-associative L1/L2 cache behavior and hit/miss statistics |

## Allocation Strategies

### First Fit

First Fit scans from the beginning of the memory block list and selects the first free block large enough for the request.

Best for:

- simple implementation
- fast average allocation in many cases

Tradeoff:

- may create small fragments near the start of memory

### Best Fit

Best Fit scans all free blocks and chooses the block that leaves the smallest leftover space.

Best for:

- using the smallest suitable space
- reducing immediate leftover waste

Tradeoff:

- may create tiny fragments that are difficult to reuse

### Worst Fit

Worst Fit scans all free blocks and chooses the largest available block.

Best for:

- keeping leftover blocks relatively large

Tradeoff:

- quickly breaks down the largest contiguous free spaces

## Requirements

- C++ compiler with C++17 support
- `g++` recommended
- `make` optional
- Bash, PowerShell, or a compatible terminal

## Build

### Option 1: Build With Make

```bash
make
```

This creates the executable in the `bin/` directory.

### Option 2: Build Manually

Use this if `make` is not installed:

```bash
mkdir -p bin
g++ -std=c++17 -Wall -Wextra -g -Iinclude \
  src/main.cpp src/SimulatorShell.cpp \
  src/allocator/MemoryManager.cpp src/allocator/FirstFit.cpp \
  src/allocator/BestFit.cpp src/allocator/WorstFit.cpp \
  src/cache/cache.cpp \
  -o bin/memsim
```

On some Windows bash environments, build an `.exe`:

```bash
mkdir -p bin
g++ -std=c++17 -Wall -Wextra -g -Iinclude \
  src/main.cpp src/SimulatorShell.cpp \
  src/allocator/MemoryManager.cpp src/allocator/FirstFit.cpp \
  src/allocator/BestFit.cpp src/allocator/WorstFit.cpp \
  src/cache/cache.cpp \
  -o bin/memsim.exe
```

## Run

Linux/macOS/WSL style:

```bash
./bin/memsim
```

Windows bash with `.exe`:

```bash
./bin/memsim.exe
```

PowerShell style:

```powershell
.\bin\memsim.exe
```

After launching, the simulator shows a prompt:

```text
>
```

Type commands at this prompt.

## Quick Demo

```text
init memory 1024
set allocator best fit
malloc 100
malloc 200
malloc 50
free 100
malloc 150
dump memory
read 64
read 65
stats
exit
```

This demo:

- creates 1024 bytes of simulated memory
- selects Best Fit allocation
- allocates three memory blocks
- frees one block
- allocates another block into the best available free space
- displays memory layout
- performs two cache reads
- prints memory and cache statistics

## Command Reference

| Command | Description | Example |
| --- | --- | --- |
| `help` | Show available commands | `help` |
| `init memory <size>` | Initialize or reset simulated RAM | `init memory 1024` |
| `set allocator <strategy>` | Select allocation strategy | `set allocator first fit` |
| `malloc <size>` | Allocate memory | `malloc 100` |
| `alloc <size>` | Alias for `malloc` | `alloc 100` |
| `free <address>` | Free a block by starting address | `free 0` |
| `dump memory` | Print memory block layout | `dump memory` |
| `read <address>` | Simulate a memory read through cache | `read 64` |
| `write <address>` | Simulate a memory write through cache | `write 64` |
| `access <address>` | Generic cache access alias | `access 64` |
| `reset cache` | Clear cache contents and cache stats | `reset cache` |
| `stats` | Print memory and cache statistics | `stats` |
| `exit` | Close the simulator | `exit` |

Supported allocator names:

```text
first
first fit
first-fit
best
best fit
best-fit
worst
worst fit
worst-fit
```

## Example Output

```text
> init memory 1024
Memory initialized with 1024 bytes.
> set allocator best fit
Allocator set to Best Fit.
> malloc 100
Allocated at address = 0x0
> malloc 200
Allocated at address = 0x64
> dump memory
Memory Dump:
[0x0000-0x0063] USED (100)
[0x0064-0x012B] USED (200)
[0x012C-0x03FF] FREE (724)
```

Cache locality example:

```text
> read 64
CPU Access 64: L1 MISS -> L2 MISS -> Physical Memory Access
> read 65
CPU Access 65: L1 HIT
```

Address `65` is an L1 hit because it belongs to the same 64-byte cache block loaded by address `64`.

## Tests

Run all included tests:

```bash
./run_tests.sh
```

Run individual test command files:

```bash
./bin/memsim < tests/fragmentation_test.txt
./bin/memsim < tests/cache_test.txt
./bin/memsim < tests/strategy_comparison_test.txt
```

If you built `memsim.exe`, use:

```bash
./bin/memsim.exe < tests/fragmentation_test.txt
./bin/memsim.exe < tests/cache_test.txt
./bin/memsim.exe < tests/strategy_comparison_test.txt
```

## Test Files

| Test File | What It Demonstrates |
| --- | --- |
| `fragmentation_test.txt` | Allocation, freeing, memory dump, and fragmentation statistics |
| `cache_test.txt` | L1/L2 cache misses and hits caused by spatial locality |
| `strategy_comparison_test.txt` | Difference between Best Fit and Worst Fit allocation choices |

## Notes and Limitations

- The simulator uses physical addresses directly.
- It does not implement virtual memory or page tables.
- It does not store real user data inside cache lines.
- It models allocation at byte granularity, so internal fragmentation is not emphasized.
- The simulator is single-process and does not model threads or process scheduling.

## Suggested Explanation

This project simulates memory management in an operating system. It maintains a list of memory blocks, supports First Fit, Best Fit, and Worst Fit allocation, merges adjacent free blocks after deallocation, calculates external fragmentation, and simulates L1/L2 cache hit-miss behavior for memory accesses.
