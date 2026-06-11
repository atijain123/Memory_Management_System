#include "../include/SimulatorShell.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

#include "../include/BestFit.h"
#include "../include/FirstFit.h"
#include "../include/WorstFit.h"

SimulatorShell::SimulatorShell() : l1Cache(1, 1024, 64, 2), l2Cache(2, 4096, 64, 4) {}

bool SimulatorShell::isRunning() const {
    return running;
}

std::string SimulatorShell::toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string SimulatorShell::readRemainingWords(std::stringstream& input) {
    std::string result;
    std::string word;
    while (input >> word) {
        if (!result.empty()) result += " ";
        result += word;
    }
    return toLower(result);
}

std::unique_ptr<AllocatorStrategy> SimulatorShell::createAllocator(const std::string& requestedName) {
    if (requestedName == "first" || requestedName == "first fit" || requestedName == "first-fit") {
        return std::make_unique<FirstFit>();
    }
    if (requestedName == "best" || requestedName == "best fit" || requestedName == "best-fit") {
        return std::make_unique<BestFit>();
    }
    if (requestedName == "worst" || requestedName == "worst fit" || requestedName == "worst-fit") {
        return std::make_unique<WorstFit>();
    }
    return nullptr;
}

void SimulatorShell::run(std::istream& input, std::ostream& output) {
    output << "Memory Management Simulator (with Cache)" << std::endl;
    output << "Type 'help' for commands." << std::endl;

    std::string line;
    while (running) {
        output << "> ";
        if (!std::getline(input, line)) break;
        executeLine(line);
    }
}

void SimulatorShell::executeLine(const std::string& line) {
    if (line.empty()) return;

    std::stringstream input(line);
    std::string command;
    input >> command;
    command = toLower(command);

    if (command == "exit" || command == "quit") {
        running = false;
    } else if (command == "help") {
        printHelp();
    } else if (command == "init") {
        handleInit(input);
    } else if (command == "set") {
        handleAllocator(input);
    } else if (command == "malloc" || command == "alloc") {
        handleMalloc(input);
    } else if (command == "free") {
        handleFree(input);
    } else if (command == "dump") {
        handleDump(input);
    } else if (command == "read" || command == "write" || command == "access") {
        handleCacheAccess(input);
    } else if (command == "stats") {
        handleStats();
    } else if (command == "reset") {
        handleResetCache(input);
    } else {
        std::cout << "Unknown command." << std::endl;
    }
}

void SimulatorShell::handleInit(std::stringstream& input) {
    std::string target;
    size_t size = 0;
    input >> target >> size;

    if (toLower(target) != "memory" || !input) {
        std::cout << "Usage: init memory <size>" << std::endl;
        return;
    }

    memoryManager.init(size);
    l1Cache.resetStats();
    l2Cache.resetStats();
}

void SimulatorShell::handleAllocator(std::stringstream& input) {
    std::string target;
    input >> target;

    if (toLower(target) != "allocator") {
        std::cout << "Usage: set allocator <first fit|best fit|worst fit>" << std::endl;
        return;
    }

    auto allocator = createAllocator(readRemainingWords(input));
    if (!allocator) {
        std::cout << "Unknown strategy." << std::endl;
        return;
    }

    memoryManager.setAllocator(std::move(allocator));
}

void SimulatorShell::handleMalloc(std::stringstream& input) {
    size_t size = 0;
    if (!(input >> size)) {
        std::cout << "Usage: malloc <size>" << std::endl;
        return;
    }

    const size_t address = memoryManager.malloc(size);
    if (address != static_cast<size_t>(-1)) {
        std::cout << "Allocated at address = 0x" << std::hex << address << std::dec << std::endl;
    }
}

void SimulatorShell::handleFree(std::stringstream& input) {
    size_t address = 0;
    if (!(input >> address)) {
        std::cout << "Usage: free <address>" << std::endl;
        return;
    }

    if (memoryManager.free(address)) {
        std::cout << "Block at " << address << " freed." << std::endl;
    }
}

void SimulatorShell::handleDump(std::stringstream& input) const {
    std::string target;
    input >> target;

    if (toLower(target) == "memory") {
        memoryManager.dumpMemory();
    } else {
        std::cout << "Usage: dump memory" << std::endl;
    }
}

void SimulatorShell::handleCacheAccess(std::stringstream& input) {
    size_t address = 0;
    if (!(input >> address)) {
        std::cout << "Usage: read <address>" << std::endl;
        return;
    }

    accessThroughCache(address);
}

void SimulatorShell::handleStats() const {
    memoryManager.printStats();
    std::cout << std::endl;
    l1Cache.printStats();
    l2Cache.printStats();
}

void SimulatorShell::handleResetCache(std::stringstream& input) {
    std::string target;
    input >> target;

    if (toLower(target) != "cache") {
        std::cout << "Usage: reset cache" << std::endl;
        return;
    }

    l1Cache.resetStats();
    l2Cache.resetStats();
    std::cout << "Cache contents and statistics reset." << std::endl;
}

void SimulatorShell::printHelp() const {
    std::cout << "Commands:\n"
              << "  init memory <size>\n"
              << "  set allocator <first fit|best fit|worst fit>\n"
              << "  malloc <size>      (alias: alloc)\n"
              << "  free <address>\n"
              << "  read <address>     (aliases: write, access)\n"
              << "  reset cache\n"
              << "  stats\n"
              << "  dump memory\n"
              << "  exit" << std::endl;
}

void SimulatorShell::accessThroughCache(size_t address) {
    std::cout << "CPU Access " << address << ": ";

    if (l1Cache.access(address)) {
        std::cout << "L1 HIT" << std::endl;
        return;
    }

    std::cout << "L1 MISS -> ";
    if (l2Cache.access(address)) {
        std::cout << "L2 HIT" << std::endl;
    } else {
        std::cout << "L2 MISS -> Physical Memory Access" << std::endl;
    }
}
