#include "direct_strategy.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

void DirectLaunchStrategy::Run(const LaunchRequest& request) const {
    if (request.path.empty()) {
        throw std::invalid_argument("Executable path is empty");
    }

    std::string command = "\"\"" + request.path.string() + "\"";

    for (const auto& arg : request.args) {
        command += " \"" + arg + "\"";
    }

    command += "\"";

    const int code = std::system(command.c_str());

    if (code == -1) {
        throw std::runtime_error("Failed to invoke command processor");
    }

    if (code != 0) {
        throw std::runtime_error(
            "Process finished with non-zero code: " + std::to_string(code)
        );
    }
}

LaunchStrategyInfo DirectLaunchStrategy::Info() const {
    return {
        "direct",
        "Direct",
        "Launches the emulator executable directly."
    };
}
