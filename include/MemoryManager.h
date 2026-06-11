#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <vector>

class AllocatorStrategy;

struct Block {
    size_t startAddress;
    size_t size;
    bool isFree;
};

class MemoryManager {
   private:
    std::vector<uint8_t> memory;
    std::list<Block> blockList;
    std::unique_ptr<AllocatorStrategy> strategy;

    void carveBlock(std::list<Block>::iterator block, size_t requestedSize);
    void coalesce();

   public:
    MemoryManager();
    ~MemoryManager();

    void init(size_t size);
    void setAllocator(std::unique_ptr<AllocatorStrategy> newStrategy);
    size_t malloc(size_t size);
    bool free(size_t address);
    void dumpMemory() const;
    void printStats() const;
};

#endif  // MEMORY_MANAGER_H
