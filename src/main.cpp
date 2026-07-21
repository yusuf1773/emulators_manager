#include <exception>
#include <iostream>

#include "application/application.hpp"

int main(int argc, char** argv) {
    try {
        Application app{"Emulator manager", "config.json"};
        app.Run(argc, argv);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}