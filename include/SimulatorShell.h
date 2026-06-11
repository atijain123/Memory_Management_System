#ifndef SIMULATOR_SHELL_H
#define SIMULATOR_SHELL_H

#include <iosfwd>
#include <memory>
#include <sstream>
#include <string>

#include "AllocatorStrategy.h"
#include "Cache.h"
#include "MemoryManager.h"

class SimulatorShell {
   private:
    MemoryManager memoryManager;
    Cache l1Cache;
    Cache l2Cache;
    bool running = true;

    static std::string toLower(std::string value);
    static std::string readRemainingWords(std::stringstream& input);
    static std::unique_ptr<AllocatorStrategy> createAllocator(const std::string& requestedName);

    void handleInit(std::stringstream& input);
    void handleAllocator(std::stringstream& input);
    void handleMalloc(std::stringstream& input);
    void handleFree(std::stringstream& input);
    void handleDump(std::stringstream& input) const;
    void handleCacheAccess(std::stringstream& input);
    void handleStats() const;
    void handleResetCache(std::stringstream& input);
    void printHelp() const;
    void accessThroughCache(size_t address);

   public:
    SimulatorShell();

    bool isRunning() const;
    void run(std::istream& input, std::ostream& output);
    void executeLine(const std::string& line);
};

#endif  // SIMULATOR_SHELL_H
