#pragma once

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "emu/persistence/file_io.h"
#include "emu/persistence/json_serialization.h"
#include "emu/persistence/storage_concepts.h"
#include "nlohmann/json.hpp"

namespace emu {

template <typename T, typename Container = std::vector<T>>
	requires IsSequenceContainer<Container, T>
class JsonStorage {
	public:
	explicit JsonStorage(const std::filesystem::path& path) : path_(path) {
		Initialize();
	}
	void Save(const Container& data) const {
		try {
			emu::persistence::WriteFile(path_, nlohmann::json(data).dump(4));
		} catch (const nlohmann::json::exception& e) {
			throw std::runtime_error("Invalid repo " + path_.string() + ": " +
									 e.what());
		}
	}

	Container Load() const {
		if (!std::filesystem::exists(path_)) {
			return {};
		}

		std::ifstream file(path_);

		if (!file) {
			throw std::runtime_error("Failed to open file: " + path_.string());
		}

		try {
			nlohmann::json json;
			file >> json;
			return json.get<Container>();
		} catch (const nlohmann::json::exception& e) {
			throw std::runtime_error("Invalid repo " + path_.string() + ": " +
									 e.what());
		}
	}

	private:
	void Initialize() {
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

}
