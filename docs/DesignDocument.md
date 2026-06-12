# Memory Management Simulator - Design Plan

## 1. Initial Idea

The idea behind this project was to create a small simulator that makes operating-system memory management easier to understand. Instead of directly working with real system memory, the project uses a simulated memory space where allocation, freeing, fragmentation, and cache access can be observed through commands.

Before implementation, I planned the project around two main parts:

- a memory allocator that shows how RAM blocks are assigned and freed
- a cache simulator that shows how repeated address access creates hits and misses

The goal was to keep the system simple enough to explain, but complete enough to demonstrate the main concepts clearly.

## 2. Design Goals

The design goals were:

- keep the simulator command-based and easy to test
- make memory layout visible after allocations and frees
- support multiple allocation strategies without changing the memory manager each time
- calculate useful statistics such as utilization and fragmentation
- simulate cache behavior using a simple L1/L2 model
- keep each major responsibility in a separate class

## 3. Planned Modules

I divided the project into these modules before implementation:

| Module | Planned Role |
| --- | --- |
| `SimulatorShell` | Read user commands and call the correct simulator component |
| `MemoryManager` | Store memory blocks, allocate, free, split, merge, and print stats |
| `AllocatorStrategy` | Common interface for allocation algorithms |
| `FirstFit`, `BestFit`, `WorstFit` | Different ways of selecting a free block |
| `Cache` | Simulate L1/L2 cache access, replacement, and statistics |
| `main.cpp` | Start the simulator only |

This design keeps `main.cpp` small and avoids mixing command parsing with allocation logic.

## 4. Memory Design Thinking

For simulated RAM, I chose a continuous byte array model:

```cpp
std::vector<uint8_t> memory;
```

This gives the simulator a fixed memory size after `init memory <size>`.

For block tracking, I chose:

```cpp
std::list<Block> blockList;
```

A list fits this project because memory blocks need to be split and adjacent free blocks need to be merged. Inserting a new free block after allocation and removing a merged block during coalescing is straightforward with a linked-list style structure.

Each block stores:

- starting address
- size
- whether it is free or used

## 5. Data Structures and Complexity

The main data structures were chosen to keep the simulator simple, readable, and suitable for memory-management operations.

| Data Structure | Used In | Why It Was Used | Main Complexity |
| --- | --- | --- | --- |
| `std::vector<uint8_t>` | Simulated physical memory | Represents a continuous byte-addressable RAM space | Initialization: O(n) |
| `std::list<Block>` | Memory block metadata | Allows easy insertion during block splitting and deletion during coalescing | Search: O(n), insert/erase at known position: O(1) |
| `std::unique_ptr<AllocatorStrategy>` | Active allocator strategy | Manages allocator object ownership safely | Strategy switch: O(1) |
| `std::vector<CacheSet>` | Cache sets | Allows direct indexing into a cache set | Set lookup: O(1) |
| `std::vector<CacheLine>` | Lines inside each cache set | Stores fixed associativity cache lines | Line scan: O(associativity) |
| `std::deque<size_t>` | FIFO replacement order | Tracks the oldest cache line in each set | push/pop: O(1) |

### Memory Operation Complexity

| Operation | Complexity | Reason |
| --- | --- | --- |
| First Fit allocation | O(n) | Scans blocks until the first suitable free block is found |
| Best Fit allocation | O(n) | Scans all blocks to find the smallest suitable free block |
| Worst Fit allocation | O(n) | Scans all blocks to find the largest suitable free block |
| Block splitting | O(1) after block is found | Inserts a new free block next to the allocated block |
| Free operation | O(n) | Searches for the block with the given starting address |
| Coalescing | O(n) | Traverses the block list and merges adjacent free blocks |
| Memory dump | O(n) | Prints every block in the block list |
| Memory stats | O(n) | Computes used memory, free memory, and largest free block |

Here, `n` is the number of memory blocks currently present in the block list.

### Cache Operation Complexity

Cache access is designed to be fast because the set index is calculated directly from the address. After the set is found, only the lines inside that set are checked.

```text
Cache access complexity = O(associativity)
```

For this simulator:

- L1 associativity is 2, so only 2 lines are checked per set.
- L2 associativity is 4, so only 4 lines are checked per set.

Because associativity is small and fixed, cache access behaves like constant time in this project.`r`n`r`n## 6. Allocation Flow Planned

The allocation flow was planned like this:

```text
malloc request
-> check selected allocator
-> find suitable free block
-> split block if larger than needed
-> mark selected part as used
-> return starting address
```

This makes the memory layout easy to explain because every allocation either consumes a full free block or splits it into used and free parts.

## 7. Free and Coalescing Flow Planned

The free flow was planned like this:

```text
free request
-> find block by starting address
-> validate address
-> reject double free
-> mark block as free
-> merge adjacent free blocks
```

Coalescing was included because without it the simulator would quickly show many separate free blocks even when they are next to each other. Merging them keeps the memory layout cleaner and demonstrates how fragmentation can be reduced.

## 8. Allocator Strategy Plan

I used the Strategy Pattern because the memory manager should not contain separate hard-coded logic for every allocation algorithm.

The plan was:

```text
MemoryManager asks selected strategy for a block
Strategy returns an iterator to the chosen free block
MemoryManager performs the actual split/allocation
```

This allows First Fit, Best Fit, and Worst Fit to be swapped at runtime using:

```text
set allocator first fit
set allocator best fit
set allocator worst fit
```

## 9. Cache Design Thinking

For the cache part, I planned a two-level cache:

```text
CPU access -> L1 -> L2 -> Physical Memory
```

The cache does not store real data. It only tracks whether a memory address maps to an existing cache line. This is enough to demonstrate hits, misses, and spatial locality.

The planned cache configuration was:

| Cache | Size | Associativity | Block Size |
| --- | ---: | ---: | ---: |
| L1 | 1024 bytes | 2-way | 64 bytes |
| L2 | 4096 bytes | 4-way | 64 bytes |

FIFO replacement was selected because it is simple to implement and easy to explain. Each set keeps an insertion order, and the oldest line is replaced first.

## 10. Command Interface Plan

The command interface was designed to act like a small simulator shell. The important commands planned were:

| Command Type | Commands |
| --- | --- |
| Setup | `init memory`, `set allocator` |
| Allocation | `malloc`, `alloc`, `free` |
| Inspection | `dump memory`, `stats` |
| Cache | `read`, `write`, `access`, `reset cache` |
| Control | `help`, `exit` |

This made it easy to manually test the project and also made it easy to create `.txt` test files containing command sequences.

## 11. Testing Plan

Testing was planned using input files instead of a separate testing framework. Since the simulator already reads commands, test files can be redirected into the executable.

Planned tests:

| Test | Purpose |
| --- | --- |
| Fragmentation test | Check allocation, free, dump, and fragmentation stats |
| Cache test | Check L1/L2 hit and miss behavior |
| Strategy comparison test | Show different choices by Best Fit and Worst Fit |

Example:

```bash
./bin/memsim < tests/cache_test.txt
```

## 12. Final Design Summary

The final design is based on separating the project into small components. The command shell handles user interaction, the memory manager handles allocation state, allocator classes decide which free block to use, and the cache class handles memory-access simulation.

This design made the project easier to build, test, explain, and extend.


