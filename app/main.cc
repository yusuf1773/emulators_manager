#include <exception>
#include <iostream>

#include "bootstrap/application.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
  SetConsoleOutputCP(65001);
#endif
  try {
    emu::Application app{"Emulator manager", "config/config.json"};
    return app.Run(argc, argv);
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}