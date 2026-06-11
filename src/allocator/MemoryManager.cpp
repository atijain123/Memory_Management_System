#include "../../include/MemoryManager.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

#include "../../include/AllocatorStrategy.h"

MemoryManager::MemoryManager() = default;

MemoryManager::~MemoryManager() = default;

void MemoryManager::init(size_t size) {
    memory.assign(size, 0);
    blockList.clear();
    if (size > 0) blockList.push_back({0, size, true});

    std::cout << "Memory initialized with " << size << " bytes." << std::endl;
}

void MemoryManager::setAllocator(std::unique_ptr<AllocatorStrategy> newStrategy) {
    strategy = std::move(newStrategy);
    if (strategy) std::cout << "Allocator set to " << strategy->getName() << "." << std::endl;
}

void MemoryManager::carveBlock(std::list<Block>::iterator block, size_t requestedSize) {
    const size_t remainingSize = block->size - requestedSize;
    if (remainingSize > 0) {
        const size_t nextAddress = block->startAddress + requestedSize;
        blockList.insert(std::next(block), {nextAddress, remainingSize, true});
        block->size = requestedSize;
    }
    block->isFree = false;
}

size_t MemoryManager::malloc(size_t size) {
    if (!strategy) {
        std::cerr << "Error: No allocator strategy set!" << std::endl;
        return -1;
    }

    if (size == 0) {
        std::cerr << "Allocation failed: Size must be greater than zero." << std::endl;
        return -1;
    }

    auto targetBlock = strategy->findFreeBlock(blockList, size);
    if (targetBlock == blockList.end()) {
        std::cerr << "Allocation failed: Not enough memory." << std::endl;
        return -1;
    }

    const size_t allocatedAddress = targetBlock->startAddress;
    carveBlock(targetBlock, size);
    return allocatedAddress;
}

bool MemoryManager::free(size_t address) {
    auto block = std::find_if(blockList.begin(), blockList.end(), [address](const Block& candidate) {
        return candidate.startAddress == address;
    });

    if (block == blockList.end()) {
        std::cerr << "Error: Invalid address " << address << std::endl;
        return false;
    }

    if (block->isFree) {
        std::cerr << "Error: Double free detected at address " << address << std::endl;
        return false;
    }

    block->isFree = true;
    coalesce();
    return true;
}

void MemoryManager::coalesce() {
    for (auto current = blockList.begin(); current != blockList.end();) {
        auto following = std::next(current);
        if (following != blockList.end() && current->isFree && following->isFree) {
            current->size += following->size;
            blockList.erase(following);
        } else {
            ++current;
        }
    }
}

void MemoryManager::dumpMemory() const {
    std::cout << "Memory Dump:" << std::endl;
    for (const auto& block : blockList) {
        std::cout << "["
                  << "0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << block.startAddress
                  << "-0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex
                  << (block.startAddress + block.size - 1) << "] " << std::dec
                  << (block.isFree ? "FREE" : "USED") << " (" << block.size << ")" << std::endl;
    }
    std::cout << std::nouppercase << std::setfill(' ');
}

void MemoryManager::printStats() const {
    const size_t totalSize = memory.size();
    size_t usedSize = 0;
    size_t totalFreeSize = 0;
    size_t maxFreeBlock = 0;

    for (const auto& block : blockList) {
        if (block.isFree) {
            totalFreeSize += block.size;
            maxFreeBlock = std::max(maxFreeBlock, block.size);
        } else {
            usedSize += block.size;
        }
    }

    const double utilization = totalSize == 0 ? 0.0 : static_cast<double>(usedSize) / totalSize * 100.0;
    const double externalFragmentation =
        totalFreeSize == 0 ? 0.0 : (1.0 - static_cast<double>(maxFreeBlock) / totalFreeSize) * 100.0;

    std::cout << "--- Memory Statistics ---" << std::endl;
    std::cout << "Total Memory:      " << totalSize << " bytes" << std::endl;
    std::cout << "Used Memory:       " << usedSize << " bytes" << std::endl;
    std::cout << "Free Memory:       " << totalFreeSize << " bytes" << std::endl;
    std::cout << "Utilization:       " << std::fixed << std::setprecision(2) << utilization << "%" << std::endl;
    std::cout << "Ext Fragmentation: " << externalFragmentation << "%" << std::endl;
    std::cout << "Largest Free Block:" << maxFreeBlock << " bytes" << std::endl;
}
