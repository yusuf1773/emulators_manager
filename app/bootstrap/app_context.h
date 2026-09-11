#pragma once

#include "emu/application/emulator_service.h"
#include "emu/application/game_service.h"
#include "emu/application/id_generator.h"
#include "emu/application/launch_profile_service.h"
#include "emu/persistence/config_data.h"
#include "emu/persistence/json_catalog_transfer.h"
#include "emu/persistence/sqlite_database.h"
#include "emu/persistence/sqlite_repositories.h"

namespace emu {

class AppContext {
public:
	explicit AppContext(ConfigData config);

	GameService& games() { return game_service_; }
	EmulatorService& emulators() { return emulator_service_; }
	LaunchProfileService& profiles() { return profile_service_; }
	GameRepository& game_repository() { return game_repository_; }
	EmulatorRepository& emulator_repository() { return emulator_repository_; }
	LaunchProfileRepository& profile_repository() {
		return profile_repository_;
	}
	HistoryRepository& history() { return history_repository_; }
	JsonCatalogTransfer& catalog_transfer() { return catalog_transfer_; }
	const ConfigData& config() const { return config_; }

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
};

}
