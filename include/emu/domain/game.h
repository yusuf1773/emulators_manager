#pragma once

#include <filesystem>
#include <string>

namespace emu {

struct Game {
	std::string name;
	std::filesystem::path path;
	std::string platform;

	bool operator==(const Game& game) const = default;
};

}
