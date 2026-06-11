#include "../../include/BestFit.h"

std::list<Block>::iterator BestFit::findFreeBlock(std::list<Block>& blockList, size_t size) {
    auto chosen = blockList.end();
    size_t bestRemainder = std::numeric_limits<size_t>::max();

    for (auto candidate = blockList.begin(); candidate != blockList.end(); ++candidate) {
        if (!candidate->isFree || candidate->size < size) continue;

        const size_t remainder = candidate->size - size;
        if (remainder < bestRemainder) {
            chosen = candidate;
            bestRemainder = remainder;
            if (bestRemainder == 0) break;
        }
    }

    return chosen;
}
