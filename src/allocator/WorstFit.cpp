#include "../../include/WorstFit.h"

std::list<Block>::iterator WorstFit::findFreeBlock(std::list<Block>& blockList, size_t size) {
    auto chosen = blockList.end();
    size_t largestBlockSeen = 0;

    for (auto candidate = blockList.begin(); candidate != blockList.end(); ++candidate) {
        if (!candidate->isFree || candidate->size < size) continue;

        if (chosen == blockList.end() || candidate->size > largestBlockSeen) {
            chosen = candidate;
            largestBlockSeen = candidate->size;
        }
    }

    return chosen;
}
