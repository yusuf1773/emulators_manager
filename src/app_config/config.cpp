#include "config.hpp"

#include <fstream>
#include <stdexcept>

namespace {
    auto ResolvePath(
        const std::filesystem::path& config_path,
        const std::filesystem::path& value
    ) {
        if (value.is_absolute()) {
            return value;
        }
        return config_path.parent_path() / value;
    }
}

Config::Config(std::filesystem::path path)
    : path_(std::move(path)) {
    if (!std::filesystem::exists(path_)) {
        SaveDefault();
    }

    Load();

    data_.emulators_path = ResolvePath(path_, data_.emulators_path);
    data_.games_path = ResolvePath(path_, data_.games_path);
}

void Config::Load() {
    std::ifstream in(path_);

    if (!in) {
        throw std::runtime_error(
            "Failed to open config: " + path_.string()
        );
    }

    try {
        nlohmann::json j;
        in >> j;

        data_ = j.get<ConfigData>();
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error(
            "Invalid config " + path_.string() + ": " + e.what()
        );
    }
}

void Config::SaveDefault() {
    const auto parent = path_.parent_path();

    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    std::ofstream out(path_);

    if (!out) {
        throw std::runtime_error(
            "Failed to create config: " + path_.string()
        );
    }

    ConfigData defaults;
    nlohmann::json j = defaults;

    try {
        out << j.dump(4);
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error(
            "Invalid config " + path_.string() + ": " + e.what()
        );
    }
}

const ConfigData& Config::GetData() const {
    return data_;
}
