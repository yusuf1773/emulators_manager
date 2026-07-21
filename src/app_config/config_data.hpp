#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

struct ConfigData {
    std::filesystem::path games_path = "games.json";
    std::filesystem::path emulators_path = "emulators.json";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ConfigData,
    games_path,
    emulators_path
)
