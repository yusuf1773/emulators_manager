#include <exception>
#include <iostream>

#include "application/application.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif
    try {
        Application app{"Emulator manager", "config/config.json"};
        app.Run(argc, argv);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}