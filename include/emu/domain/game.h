#pragma once

#include <filesystem>
#include <string>

#include "emu/domain/entity_ids.h"

namespace emu {

struct Game {
	GameId id;
	std::string name;
	std::filesystem::path content_path;
	std::string platform;

	bool operator==(const Game& game) const = default;
};

}
