#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace emu {

struct LaunchRequest {
	std::filesystem::path executable;
	std::vector<std::string> arguments;
};

class ProcessRunner {
	public:
	virtual ~ProcessRunner() = default;

	virtual void Run(const LaunchRequest& request) = 0;
};

}
