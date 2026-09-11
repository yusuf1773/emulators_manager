#include "emu/persistence/config.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <utility>

#include "nlohmann/json.hpp"

namespace emu {

namespace {
auto ResolvePath(const std::filesystem::path& config_path,
				 const std::filesystem::path& value) {
	if (value.is_absolute()) {
		return value;
	}
	return config_path.parent_path() / value;
}
}

Config::Config(std::filesystem::path path) : path_(std::move(path)) {
	if (!std::filesystem::exists(path_)) {
		SaveDefault();
	}

	Load();

	data_.database_path = ResolvePath(path_, data_.database_path);
	data_.emulators_path = ResolvePath(path_, data_.emulators_path);
	data_.export_path = ResolvePath(path_, data_.export_path);
	data_.games_path = ResolvePath(path_, data_.games_path);
	data_.history_path = ResolvePath(path_, data_.history_path);
}

void Config::Load() {
	std::ifstream in(path_);

	if (!in) {
		throw std::runtime_error("Failed to open config: " + path_.string());
	}

	try {
		nlohmann::json j;
		in >> j;

		data_.database_path = j.value("database_path", data_.database_path);
		data_.games_path = j.value("games_path", data_.games_path);
		data_.emulators_path = j.value("emulators_path", data_.emulators_path);
		data_.export_path = j.value("export_path", data_.export_path);
		data_.history_path = j.value("history_path", data_.history_path);
		data_.history_max_size =
			j.value("history_max_size", data_.history_max_size);
	} catch (const nlohmann::json::exception& e) {
		throw std::runtime_error("Invalid config " + path_.string() + ": " +
								 e.what());
	}
}

void Config::SaveDefault() {
	const auto parent = path_.parent_path();

	if (!parent.empty()) {
		std::filesystem::create_directories(parent);
	}

	std::ofstream out(path_);

	if (!out) {
		throw std::runtime_error("Failed to create config: " + path_.string());
	}

	ConfigData defaults;
	nlohmann::json j{{"database_path", defaults.database_path},
					 {"games_path", defaults.games_path},
					 {"emulators_path", defaults.emulators_path},
					 {"export_path", defaults.export_path},
					 {"history_path", defaults.history_path},
					 {"history_max_size", defaults.history_max_size}};

	try {
		out << j.dump(4);
	} catch (const nlohmann::json::exception& e) {
		throw std::runtime_error("Invalid config " + path_.string() + ": " +
								 e.what());
	}
}

const ConfigData& Config::GetData() const { return data_; }

}
