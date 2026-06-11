#ifndef WORST_FIT_H
#define WORST_FIT_H

#include "AllocatorStrategy.h"
#include "MemoryManager.h"

class WorstFit : public AllocatorStrategy {
   public:
    std::list<Block>::iterator findFreeBlock(std::list<Block>& blockList, size_t size) override;
    std::string getName() const override { return "Worst Fit"; }
};

#endif  // WORST_FIT_H
