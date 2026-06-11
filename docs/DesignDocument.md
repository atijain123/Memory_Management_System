# Memory Management Simulator - Design Document

## 1. Overview
This project simulates a memory management system in user space, featuring a contiguous physical memory allocator and a multilevel CPU cache hierarchy. The implementation is split into three major responsibilities:

* `SimulatorShell` owns the command-line interface and dispatches commands.
* `MemoryManager` owns the simulated RAM and block metadata.
* `Cache` owns cache set/line state and hit/miss accounting.

## 2. Memory Layout & Assumptions
* **Physical Memory:** Modeled as a `std::vector<uint8_t>` of configurable size.
* **Metadata:** Memory blocks are tracked using a **Doubly Linked List** of `Block` structures.
    * Each `Block` contains `startAddress`, `size`, and `isFree` flags.
    * **Assumption:** The overhead of block headers is stored separately from the physical memory array.
* **Addressing:** The simulator uses physical addressing directly (Virtual Memory is disabled).

## 3. Allocation Strategy Implementations
The system uses the **Strategy Pattern** to switch algorithms dynamically at runtime. Strategy objects are owned through `std::unique_ptr`, so changing the active allocator does not leak the previous strategy object.

### A. First Fit
* **Logic:** Iterates through the block list from the beginning. Returns the *first* free block where `block.size >= requested_size`.
* **Pros:** Fast allocation (O(n) but often O(1)).
* **Cons:** Accumulates "splinters" (small fragments) at the beginning of memory.

### B. Best Fit
* **Logic:** Iterates through the *entire* list to find the free block with the smallest size difference (`block.size - requested_size`).
* **Pros:** Minimizes wasted space in the chosen block.
* **Cons:** Slower (always O(n)); tends to create tiny, unusable fragments.

### C. Worst Fit
* **Logic:** Iterates through the *entire* list to find the largest free block.
* **Pros:** Leftover chunks are likely large enough to be useful later.
* **Cons:** Slower (always O(n)); breaks up large contiguous blocks quickly.

## 4. Cache Hierarchy & Replacement Policy
The system implements a two-level cache hierarchy (L1 and L2) acting as a controller over the physical memory.

* **Architecture:**
    * **L1 Cache:** 1KB, 2-way set associative, 64B block size.
    * **L2 Cache:** 4KB, 4-way set associative, 64B block size.
* **Flow:** CPU Request -> Check L1 -> (Miss) -> Check L2 -> (Miss) -> Access Physical Memory.
* **Replacement Policy:** **FIFO (First-In, First-Out)**.
    * Each set stores an insertion-order queue. Empty lines are filled first. Once a set is full, the oldest line index is popped from the queue and reused for the new cache block.

## 5. CLI Interface Design
The simulator provides a command-line interface to interact with the memory system.

| Command | Description | Example |
| :--- | :--- | :--- |
| `init memory <size>` | Initialize physical RAM size | `init memory 1024` |
| `set allocator <strat>` | Switch allocation strategy | `set allocator best fit` |
| `malloc <size>` | Allocate memory block | `malloc 100` |
| `alloc <size>` | Alias for `malloc` | `alloc 100` |
| `free <address>` | Free memory at specific address | `free 0` |
| `read <address>` | Simulate CPU read (triggers Cache) | `read 64` |
| `write <address>` | Simulate CPU write (same cache path) | `write 64` |
| `access <address>` | Generic cache access alias | `access 64` |
| `reset cache` | Clear cache contents and hit/miss counters | `reset cache` |
| `stats` | Show Fragmentation & Cache Hit Rates | `stats` |
| `dump memory` | Visualize memory layout | `dump memory` |
| `exit` | Quit the simulator | `exit` |

## 6. Limitations & Simplifications
1.  **No Virtual Memory:** `malloc` returns physical addresses directly.
2.  **No Real Data Storage:** The cache tracks *tags* and *valid bits* but does not store or return actual data values (only simulation of hits/misses).
3.  **Single Process:** The simulator assumes a single execution context (no context switching).
4.  **Byte-Level Allocation:** Allocations are split exactly at the requested size, so internal fragmentation is not modeled.
