#pragma once

#include <filesystem>
#include <vector>
#include <fstream>

#include "../game/game.hpp"


template <typename T>
class GameRepository {
public:
    explicit GameRepository(std::filesystem::path path)
        : path_(std::move(path)) {
        Initialize();
    }

    std::vector<T> GetAll() const {
        return Load();
    }

    void Add(const T& value) {
        auto values = Load();
        if (std::find(values.begin(), values.end(), value) != values.end()) {
            throw std::runtime_error("Duplicate value to add");
        }
        values.push_back(value);
        Save(values);
    }

private:
    std::vector<T> Load() const {
        if (!std::filesystem::exists(path_)) {
            return {};
        }

        std::ifstream file(path_);

        if (!file) {
            throw std::runtime_error(
                "Failed to open file: " + path_.string()
            );
        }

        try {
            nlohmann::json json;
            file >> json;
            return json.get<std::vector<T>>();
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error(
                "Invalid repo " + path_.string() + ": " + e.what()
            );
        }
    }

    void Save(const std::vector<T>& values) const {
        std::ofstream file(path_, std::ios::trunc);

        if (!file) {
            throw std::runtime_error(
                "Failed to open file: " + path_.string()
            );
        }

        try {
            file << nlohmann::json(values).dump(4);
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error(
                "Invalid repo " + path_.string() + ": " + e.what()
            );
        }
    }

    void Initialize() const {
        if (!std::filesystem::exists(path_)) {
            Save({});
            return;
        }

        if (std::filesystem::file_size(path_) == 0) {
            Save({});
        }
    }
private:
    std::filesystem::path path_;
};
