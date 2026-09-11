#pragma once

#include <chrono>
#include <string>
#include <utility>

namespace emu {

struct CommandHistoryEntry {
	std::string command{};
	std::chrono::system_clock::time_point timestamp;

	CommandHistoryEntry() = default;

	explicit CommandHistoryEntry(std::string line)
		: command(std::move(line)),
		  timestamp(std::chrono::system_clock::now()) {}

	CommandHistoryEntry(std::string line,
						std::chrono::system_clock::time_point time)
		: command(std::move(line)), timestamp(time) {}
};

}
