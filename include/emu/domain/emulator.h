#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "emu/domain/entity_ids.h"

namespace emu {

struct Emulator {
	EmulatorId id;
	std::string name;
	std::filesystem::path executable_path;
	std::string platform;
	std::vector<std::string> base_arguments;

	bool operator==(const Emulator& emulator) const = default;
};

}
