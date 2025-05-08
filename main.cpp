#include "main.h"

int main() {
    try {
        // Initialize and start the OSIM system
        SystemInterface system;
        system.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    return 0;
}