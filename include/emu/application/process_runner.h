#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "emu/application/application_error.h"

namespace emu {

struct LaunchRequest {
	std::filesystem::path executable;
	std::vector<std::string> arguments;
	std::filesystem::path working_directory;
};

struct ProcessResult {
	int exit_code;
};

class ProcessRunner {
public:
	virtual ~ProcessRunner() = default;

	virtual Result<ProcessResult> Run(const LaunchRequest& request) = 0;
};

}
