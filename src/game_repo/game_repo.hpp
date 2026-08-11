#pragma once

#include <filesystem>
#include <vector>

#include "constrains.hpp"
#include "json_storage.hpp"

template <typename T, typename Storage = JsonStorage<T>>
requires IsStorage<Storage, T, std::vector<T>>
class GameRepository {
public:
    explicit GameRepository(const std::filesystem::path& path, std::function<const std::string&(const T&)> getter)
        : storage_(path), values_(storage_.Load()), getter_(std::move(getter)) {}

    const std::vector<T>& GetAll() const {
        return values_;
    }

    void Add(const T& value, bool allow_replace = false) {
        const std::string name = getter_(value);
        auto it = std::find_if(values_.begin(), values_.end(), [&] (const T& el) {
            return getter_(el) == name;
        });
        if (it != values_.end()) {
            if (!allow_replace) {
                throw std::runtime_error("Duplicate value to add");
            } else {
                *it = value;
            }
        } else {
            values_.push_back(value);
            storage_.Save(values_);
        }
    }

    void RemoveByName(const std::string& name) {
        auto it = std::find_if(values_.begin(), values_.end(), [&] (const T& el) {
            return getter_(el) == name;
        });
        if (it == values_.end()) {
            throw std::runtime_error("Not found in repository " + name);
        }
        values_.erase(it);
        storage_.Save(values_);
    }

    const T& GetById(const std::string& name) const {
        auto it = std::find_if(values_.begin(), values_.end(), [&] (const T& el) {
            return getter_(el) == name;
        });
        if (it == values_.end()) {
            throw std::runtime_error("Not found in repository " + name);
        }
        return *it;
    }
private:
    Storage storage_;
    std::vector<T> values_;
    std::function<const std::string&(const T&)> getter_;
};
