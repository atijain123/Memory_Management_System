#include "../../include/FirstFit.h"

#include <algorithm>

std::list<Block>::iterator FirstFit::findFreeBlock(std::list<Block>& blockList, size_t size) {
    return std::find_if(blockList.begin(), blockList.end(), [size](const Block& block) {
        return block.isFree && block.size >= size;
    });
}
