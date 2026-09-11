#pragma once

#include <filesystem>

#include "emu/application/emulator_repository.h"
#include "emu/application/emulator_service.h"
#include "emu/application/game_repository.h"
#include "emu/application/game_service.h"
#include "emu/application/launch_profile_repository.h"
#include "emu/application/launch_profile_service.h"

namespace emu {

class JsonCatalogTransfer {
public:
	JsonCatalogTransfer(GameRepository& games, EmulatorRepository& emulators,
						LaunchProfileRepository& profiles,
						GameService& game_service,
						EmulatorService& emulator_service,
						LaunchProfileService& profile_service);

	Result<void> ImportLegacyIfEmpty(
		const std::filesystem::path& games_path,
		const std::filesystem::path& emulators_path);
	Result<void> Export(const std::filesystem::path& path) const;

private:
	GameRepository& games_;
	EmulatorRepository& emulators_;
	LaunchProfileRepository& profiles_;
	GameService& game_service_;
	EmulatorService& emulator_service_;
	LaunchProfileService& profile_service_;
};

}
