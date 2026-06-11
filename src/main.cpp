#include <exception>
#include <iostream>

#include "../include/SimulatorShell.h"

int main() {
    try {
        SimulatorShell shell;
        shell.run(std::cin, std::cout);
    } catch (const std::exception& error) {
        std::cerr << "Fatal simulator error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
