#include "SystemInterface.h"
#include <iostream>
#include <stdexcept>


using namespace std;

int main() {
    try {
        // Initialize and start the OSIM system
        SystemInterface system;
        system.start();
    }
    catch (const exception& e) {
        cerr << "Fatal Error: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "Unknown error occurred" << endl;
        return 1;
    }
    return 0;
}
