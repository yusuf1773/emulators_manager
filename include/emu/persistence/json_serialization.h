#pragma once

#include <chrono>

#include "emu/domain/command_history_entry.h"
#include "emu/domain/emulator.h"
#include "emu/domain/game.h"
#include "emu/persistence/config_data.h"
#include "nlohmann/json.hpp"

namespace emu {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Game, name, path, platform)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Emulator, name, path, platform,
								   launch_strategy_id, args)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData, games_path, emulators_path,
								   history_path, history_max_size)

inline void to_json(nlohmann::json& json, const CommandHistoryEntry& entry) {
	const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
							 entry.timestamp.time_since_epoch())
							 .count();
	json = {{"command", entry.command}, {"timestamp", seconds}};
}

inline void from_json(const nlohmann::json& json, CommandHistoryEntry& entry) {
	json.at("command").get_to(entry.command);
	entry.timestamp = std::chrono::system_clock::time_point(
		std::chrono::seconds(json.at("timestamp").get<long long>()));
}

}
