#include "../../include/Cache.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

Cache::Cache(size_t level, size_t size, size_t blockSize, size_t associativity)
    : level(level), size(size), blockSize(blockSize), associativity(associativity) {
    if (blockSize == 0 || associativity == 0 || size < blockSize * associativity) {
        throw std::invalid_argument("Invalid cache configuration.");
    }

    numSets = size / (associativity * blockSize);
    numOffsetBits = bitWidth(blockSize);
    numIndexBits = bitWidth(numSets);

    sets.assign(numSets, CacheSet{});
    for (auto& set : sets) set.lines.resize(associativity);

    std::cout << "Cache L" << level << " initialized: " << size << "B total, " << numSets << " sets, " << associativity
              << "-way." << std::endl;
}

size_t Cache::bitWidth(size_t value) {
    size_t bits = 0;
    while (value > 1) {
        value >>= 1;
        ++bits;
    }
    return bits;
}

void Cache::installLine(CacheSet& set, size_t tag) {
    for (size_t index = 0; index < set.lines.size(); ++index) {
        if (!set.lines[index].valid) {
            set.lines[index] = {true, static_cast<uint32_t>(tag)};
            set.insertionOrder.push_back(index);
            return;
        }
    }

    const size_t victimIndex = set.insertionOrder.front();
    set.insertionOrder.pop_front();
    set.lines[victimIndex] = {true, static_cast<uint32_t>(tag)};
    set.insertionOrder.push_back(victimIndex);
}

bool Cache::access(size_t address) {
    const size_t indexMask = (static_cast<size_t>(1) << numIndexBits) - 1;
    const size_t setIndex = (address >> numOffsetBits) & indexMask;
    const size_t tag = address >> (numOffsetBits + numIndexBits);

    auto& set = sets[setIndex];
    for (const auto& line : set.lines) {
        if (line.valid && line.tag == tag) {
            hits++;
            return true;
        }
    }

    misses++;
    installLine(set, tag);
    return false;
}

void Cache::printStats() const {
    const size_t totalAccesses = hits + misses;
    const double hitRate = totalAccesses == 0 ? 0.0 : static_cast<double>(hits) / totalAccesses * 100.0;

    std::cout << "L" << level << " Cache Stats: "
              << "Hits: " << hits << ", "
              << "Misses: " << misses << ", "
              << "Hit Rate: " << std::fixed << std::setprecision(2) << hitRate << "%" << std::endl;
}

void Cache::resetStats() {
    hits = 0;
    misses = 0;
    for (auto& set : sets) {
        for (auto& line : set.lines) line = {};
        set.insertionOrder.clear();
    }
}
