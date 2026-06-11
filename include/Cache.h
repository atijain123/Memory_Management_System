#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <deque>
#include <iostream>
#include <vector>

struct CacheLine {
    bool valid = false;
    uint32_t tag = 0;
};

struct CacheSet {
    std::vector<CacheLine> lines;
    std::deque<size_t> insertionOrder;
};

class Cache {
   private:
    size_t level;
    size_t size;
    size_t blockSize;
    size_t associativity;

    size_t numSets;
    size_t numOffsetBits;
    size_t numIndexBits;

    std::vector<CacheSet> sets;

    // Statistics
    size_t hits = 0;
    size_t misses = 0;

    static size_t bitWidth(size_t value);
    void installLine(CacheSet& set, size_t tag);

   public:
    Cache(size_t level, size_t size, size_t blockSize, size_t associativity);

    bool access(size_t address);
    void printStats() const;
    void resetStats();
};

#endif  // CACHE_H
