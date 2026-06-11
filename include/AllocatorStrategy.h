#ifndef ALLOCATOR_STRATEGY_H
#define ALLOCATOR_STRATEGY_H

#include <cstddef>
#include <list>

#include "MemoryManager.h"

class AllocatorStrategy {
   public:
    virtual ~AllocatorStrategy() = default;

    virtual std::list<Block>::iterator findFreeBlock(std::list<Block>& blockList, size_t size) = 0;
    virtual std::string getName() const = 0;
};

#endif  // ALLOCATOR_STRATEGY_H
