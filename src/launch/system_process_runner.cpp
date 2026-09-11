#include "emu/launch/system_process_runner.h"

#include <cstdlib>
#include <stdexcept>
#include <string>

namespace emu {

void SystemProcessRunner::Run(const LaunchRequest& request) {
	if (request.executable.empty()) {
		throw std::invalid_argument("Executable path is empty");
	}

	std::string command = "\"\"" + request.executable.string() + "\"";
	for (const auto& argument : request.arguments) {
		command += " \"" + argument + "\"";
	}
	command += "\"";

	const int code = std::system(command.c_str());
	if (code == -1) {
		throw std::runtime_error("Failed to invoke command processor");
	}
	if (code != 0) {
		throw std::runtime_error("Process finished with non-zero code: " +
								 std::to_string(code));
	}
}

}
