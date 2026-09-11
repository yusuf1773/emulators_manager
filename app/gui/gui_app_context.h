#pragma once

#include <filesystem>

#include "emu/application/emulator_service.h"
#include "emu/application/game_service.h"
#include "emu/application/id_generator.h"
#include "emu/application/launch_profile_service.h"
#include "emu/persistence/config_data.h"
#include "emu/persistence/json_catalog_transfer.h"
#include "emu/persistence/sqlite_database.h"
#include "emu/persistence/sqlite_repositories.h"
#include "error_notifier.h"
#include "models/emulator_list_model.h"
#include "models/game_list_model.h"

namespace emu {

class GuiAppContext final {
public:
	explicit GuiAppContext(const std::filesystem::path& config_path);

	GameListModel& game_list_model() { return game_list_model_; }
	EmulatorListModel& emulator_list_model() { return emulator_list_model_; }
	ErrorNotifier& errors() { return errors_; }

private:
	ConfigData config_;
	SqliteDatabase database_;
	SqliteGameRepository game_repository_;
	SqliteEmulatorRepository emulator_repository_;
	SqliteLaunchProfileRepository profile_repository_;
	SqliteHistoryRepository history_repository_;
	RandomIdGenerator id_generator_;
	GameService game_service_;
	EmulatorService emulator_service_;
	LaunchProfileService profile_service_;
	JsonCatalogTransfer catalog_transfer_;
	ErrorNotifier errors_;
	GameListModel game_list_model_;
	EmulatorListModel emulator_list_model_;
};

}
