#pragma once

#include <filesystem>
#include <vector>
#include <fstream>

#include <nlohmann/json.hpp>

template <typename T>
class GameRepository {
public:
    explicit GameRepository(std::filesystem::path path, std::function<const std::string&(const T&)> getter)
        : path_(std::move(path)), getter_(std::move(getter)) {
        Initialize();
    }

    std::vector<T> GetAll() const {
        return Load();
    }

    void Add(const T& value, bool allow_replace = false) {
        auto values = Load();
        const std::string name = getter_(value);
        auto it = std::find_if(values.begin(), values.end(), [&] (const T& el) {
            return getter_(el) == name;
        });
        if (it != values.end()) {
            if (!allow_replace) {
                throw std::runtime_error("Duplicate value to add");
            } else {
                *it = value;
            }
        } else {
            values.push_back(value);
            Save(values);
        }
    }

    void RemoveByName(const std::string& name) {
        auto values = Load();
        auto it = std::find_if(values.begin(), values.end(), [&] (const T& el) {
            return getter_(el) == name;
        });
        if (it == values.end()) {
            throw std::runtime_error("Not found in repository " + name);
        }
        values.erase(it);
        Save(values);
    }

    T GetById(const std::string& name) const {
        auto values = Load();
        auto it = std::find_if(values.begin(), values.end(), [&] (const T& el) {
            return getter_(el) == name;
        });
        if (it == values.end()) {
            throw std::runtime_error("Not found in repository " + name);
        }
        return *it;
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
    std::function<const std::string&(const T&)> getter_;
};
