#pragma once

#include <filesystem>

#include "config_data.hpp"

class Config {
public:
    explicit Config(std::filesystem::path path);

    const ConfigData& GetData() const;

private:
    void Load();
    void SaveDefault();

    std::filesystem::path path_;
    ConfigData data_;
};
