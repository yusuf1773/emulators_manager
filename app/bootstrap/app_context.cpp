#include "bootstrap/app_context.h"

#include <string>
#include <utility>

namespace emu {

AppContext::AppContext(ConfigData config)
	: config_data_(std::move(config)),
	  game_repo_(
		  config_data_.games_path,
		  [](const Game& game) -> const std::string& { return game.name; }),
	  emu_repo_(
		  config_data_.emulators_path,
		  [](const Emulator& emu) -> const std::string& { return emu.name; }),
	  history_repo_(config_data_.history_path, config_data_.history_max_size) {}

JsonRepository<Game>& AppContext::GetGameRepo() { return game_repo_; }

JsonRepository<Emulator>& AppContext::GetEmulatorsRepo() { return emu_repo_; }

JsonHistoryRepository& AppContext::GetCommandsHistoryRepo() {
	return history_repo_;
}

const ConfigData& AppContext::GetConfig() const { return config_data_; }

}
