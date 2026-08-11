#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

struct ConfigData {
    std::filesystem::path games_path = "games.json";
    std::filesystem::path emulators_path = "emulators.json";
    std::filesystem::path history_path = "history.json";
    size_t history_max_size = 15;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ConfigData,
    games_path,
    emulators_path,
    history_path,
    history_max_size
)
