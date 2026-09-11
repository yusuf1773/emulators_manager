#pragma once

#include <cstddef>
#include <filesystem>

namespace emu {

struct ConfigData {
	std::filesystem::path database_path = "emulators.db";
	std::filesystem::path games_path = "games.json";
	std::filesystem::path emulators_path = "emulators.json";
	std::filesystem::path export_path = "catalog.json";
	std::filesystem::path history_path = "history.json";
	std::size_t history_max_size = 15;
};

}
