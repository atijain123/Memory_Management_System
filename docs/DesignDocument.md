# Memory Management Simulator - Design Document

## 1. Design Intention

The main idea behind this project was to build a small but clear simulator that explains how memory management works inside an operating system. Instead of making a very large system with too many features, the design focuses on a few important concepts and makes them visible through command-line output.

The project was planned around two questions:

1. How does an operating system allocate and free contiguous memory blocks?
2. How does a CPU-style cache respond when memory addresses are accessed repeatedly?

Because this is an educational simulator, the design favors readability, step-by-step behavior, and easy testing over low-level hardware accuracy.

## 2. Initial Design Approach

At the beginning, the simulator was divided into three logical parts:

| Part | Design Purpose |
| --- | --- |
| Command shell | Accept commands from the user and display results |
| Memory manager | Maintain simulated RAM and block metadata |
| Cache simulator | Track L1/L2 cache accesses, hits, and misses |

This separation was chosen so that the command-line interface does not directly contain all allocation and cache logic. The shell only interprets commands, while the actual work is handled by separate classes.

## 3. Why a Command-Line Simulator

A command-line interface was selected because it makes every operation visible. For example, the user can initialize memory, allocate blocks, free blocks, dump memory, and then immediately see how the internal memory layout changed.

This is useful for learning because the project does not hide the process behind a graphical interface. Each command maps directly to one memory-management action.

Example flow:

```text
init memory 1024
set allocator best fit
malloc 100
malloc 200
free 100
dump memory
stats
```

This flow shows allocation, deallocation, fragmentation, and memory statistics in a simple sequence.

## 4. Memory Representation Design

The simulated physical memory is represented using:

```cpp
std::vector<uint8_t> memory;
```

This vector represents a continuous byte-addressable memory region. The simulator does not store meaningful user data in this vector; it mainly uses the vector size to represent total available memory.

The metadata for memory blocks is stored separately using:

```cpp
std::list<Block> blockList;
```

Each block stores:

- start address
- block size
- free/used status

A linked list was chosen because allocation and deallocation often require inserting or merging neighboring blocks. Using a list makes block splitting and coalescing easier to express.

## 5. Block Splitting Design

When a user requests memory, the selected allocator strategy returns a suitable free block. If the free block is larger than the requested size, it is split into two parts:

1. a used block of the requested size
2. a remaining free block

This design was used because it clearly demonstrates contiguous allocation and external fragmentation.

Example:

```text
Before malloc 100:
[0-1023] FREE

After malloc 100:
[0-99] USED
[100-1023] FREE
```

## 6. Coalescing Design

When a block is freed, adjacent free blocks should be merged. This is called coalescing.

The design decision was to call coalescing immediately after every successful `free` operation. This keeps the memory layout cleaner and prevents unnecessary fragmentation from building up when neighboring blocks are free.

Example:

```text
[0-99] FREE
[100-199] FREE
```

After coalescing:

```text
[0-199] FREE
```

## 7. Allocation Strategy Design

The allocator algorithms were designed using the Strategy Pattern. This means the memory manager does not need to know the internal details of First Fit, Best Fit, or Worst Fit. It only asks the selected strategy to find a suitable free block.

This design was chosen because:

- new allocation strategies can be added later
- the memory manager remains simpler
- the algorithms can be compared using the same memory operations

### First Fit

First Fit was included because it is simple and commonly taught in operating-system memory allocation. It scans from the beginning and selects the first block large enough for the request.

### Best Fit

Best Fit was included to show a strategy that tries to reduce leftover space in the selected block. It scans all free blocks and chooses the closest match.

### Worst Fit

Worst Fit was included as a contrast to Best Fit. It selects the largest available block so that the remaining free block may still be useful for future allocations.

## 8. Fragmentation Statistics Design

The simulator calculates memory statistics to help the user understand the current state of memory.

The main statistics are:

- total memory
- used memory
- free memory
- utilization percentage
- largest free block
- external fragmentation percentage

External fragmentation is calculated by comparing total free memory with the largest single free block:

```text
External Fragmentation = 1 - (Largest Free Block / Total Free Memory)
```

This was added because simply knowing total free memory is not enough. A system may have enough free memory in total, but it may be split into small blocks.

## 9. Cache Design

The cache simulator was added to connect memory allocation with memory access behavior. The cache does not store actual data values. Instead, it tracks whether a memory address would be a cache hit or miss.

The design uses two cache levels:

| Cache | Size | Associativity | Block Size | Replacement |
| --- | ---: | ---: | ---: | --- |
| L1 | 1024 bytes | 2-way | 64 bytes | FIFO |
| L2 | 4096 bytes | 4-way | 64 bytes | FIFO |

The cache access flow is:

```text
CPU access -> L1 cache -> L2 cache -> physical memory
```

This structure was chosen because it is simple enough to implement but still demonstrates the idea of a multilevel cache hierarchy.

## 10. FIFO Replacement Design

FIFO replacement was selected because it is easy to understand and explain. Each cache set keeps an insertion-order queue. When the set is full, the oldest inserted cache line is replaced.

The design avoids more complex policies like LRU because the goal of the project is to demonstrate the cache hierarchy clearly, not to implement every possible replacement policy.

## 11. Command Handling Design

The command handling is placed in `SimulatorShell`. This class reads user commands and calls the correct part of the simulator.

This design keeps `main.cpp` small. The main function only creates the shell and starts it. This makes the project easier to maintain because command parsing is separated from memory and cache logic.

Supported command groups:

| Group | Examples |
| --- | --- |
| Memory setup | `init memory 1024` |
| Allocator selection | `set allocator first fit` |
| Allocation/free | `malloc 100`, `free 0` |
| Inspection | `dump memory`, `stats` |
| Cache access | `read 64`, `write 64` |
| Cache control | `reset cache` |

## 12. Error Handling Design

Basic validation was added for common mistakes:

- allocation before setting an allocator
- zero-size allocation
- freeing an invalid address
- double free detection
- invalid command usage

The goal was not to make a complex production-level parser, but to prevent common incorrect operations from silently changing the simulator state.

## 13. Testing Design

The project uses text files as scripted command inputs. This was chosen because the simulator itself is command-driven.

Test files include:

| Test File | Purpose |
| --- | --- |
| `fragmentation_test.txt` | Shows allocation, freeing, memory dump, and fragmentation |
| `cache_test.txt` | Shows cache locality and hit/miss behavior |
| `strategy_comparison_test.txt` | Shows the difference between Best Fit and Worst Fit |

This design makes testing simple because each test file is just a sequence of commands that can be redirected into the executable.

## 14. Important Design Decisions

| Decision | Reason |
| --- | --- |
| Use contiguous memory model | Easy to visualize allocation and fragmentation |
| Store metadata separately | Keeps simulation simple and readable |
| Use linked list for blocks | Makes splitting and coalescing straightforward |
| Use Strategy Pattern | Allows allocator algorithms to be switched easily |
| Use FIFO cache replacement | Simple and explainable cache policy |
| Use command-line interface | Makes simulator behavior transparent |
| Keep virtual memory out of scope | Avoids making the project too complex |

## 15. Features Not Included

Some features were intentionally not included because they would make the project much larger:

- virtual memory
- paging
- page replacement algorithms
- Buddy Memory Allocation System
- real process scheduling
- actual data storage inside cache lines
- graphical user interface

These can be added as future improvements, but they were not part of the current design.

## 16. Final Design Summary

The final design is a modular simulator with separate components for command handling, memory allocation, allocation strategies, and cache simulation. The project was designed to make operating-system memory concepts visible through simple commands and readable output.

The main focus is not just to allocate memory, but to show how memory layout changes over time, how fragmentation appears, how coalescing helps, and how cache locality affects memory access results.
