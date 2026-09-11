#pragma once

#include <cstddef>
#include <filesystem>

namespace emu {

struct ConfigData {
	std::filesystem::path games_path = "games.json";
	std::filesystem::path emulators_path = "emulators.json";
	std::filesystem::path history_path = "history.json";
	std::size_t history_max_size = 15;
};

}
