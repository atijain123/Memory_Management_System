#ifndef FIRST_FIT_H
#define FIRST_FIT_H

#include "AllocatorStrategy.h"
#include "MemoryManager.h"  // For the Block definition

class FirstFit : public AllocatorStrategy {
   public:
    std::list<Block>::iterator findFreeBlock(std::list<Block>& blockList, size_t size) override;
    std::string getName() const override { return "First Fit"; }
};

#endif  // FIRST_FIT_H
