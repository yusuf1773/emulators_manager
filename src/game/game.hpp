#pragma once

#include <string>
#include <filesystem>

#include <nlohmann/json.hpp>

struct Game {
    std::string name;
    std::filesystem::path path;
    std::string platform;

    bool operator==(const Game& game) const = default;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Game,
    name,
    path,
    platform
)
