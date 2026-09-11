#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "emu/domain/entity_ids.h"

namespace emu {

struct LaunchProfile {
	LaunchProfileId id;
	std::string name;
	GameId game_id;
	EmulatorId emulator_id;
	std::string strategy_id;
	std::vector<std::string> arguments;
	std::filesystem::path working_directory;
	bool is_default = false;

	bool operator==(const LaunchProfile& profile) const = default;
};

}
