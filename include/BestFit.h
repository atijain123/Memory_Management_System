#ifndef BEST_FIT_H
#define BEST_FIT_H

#include <limits>

#include "AllocatorStrategy.h"
#include "MemoryManager.h"

class BestFit : public AllocatorStrategy {
   public:
    std::list<Block>::iterator findFreeBlock(std::list<Block>& blockList, size_t size) override;
    std::string getName() const override { return "Best Fit"; }
};

#endif  // BEST_FIT_H
