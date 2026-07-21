#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <nlohmann/json.hpp>

struct Emulator {
    std::string name;
    std::filesystem::path path;
    std::string platform;
    std::string launch_strategy_id;
    std::vector<std::string> args;

    bool operator==(const Emulator& emulator) const = default;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Emulator,
    name,
    path,
    platform,
    launch_strategy_id,
    args
)
