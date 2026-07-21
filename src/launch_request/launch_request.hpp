#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct LaunchRequest {
    std::filesystem::path path;
    std::vector<std::string> args;
};
