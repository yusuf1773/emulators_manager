#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "emu/application/emulator_repository.h"
#include "emu/application/game_repository.h"
#include "emu/application/id_generator.h"
#include "emu/application/launch_profile_repository.h"

namespace emu {

class LaunchProfileService {
public:
	LaunchProfileService(LaunchProfileRepository& profiles,
						 GameRepository& games, EmulatorRepository& emulators,
						 IdGenerator& ids);

	Result<LaunchProfile> Create(std::string name, GameId game_id,
								 EmulatorId emulator_id,
								 std::string strategy_id,
								 std::vector<std::string> arguments,
								 std::filesystem::path working_directory,
								 bool is_default);
	Result<LaunchProfile> Update(LaunchProfileId id, std::string name,
								 GameId game_id, EmulatorId emulator_id,
								 std::string strategy_id,
								 std::vector<std::string> arguments,
								 std::filesystem::path working_directory,
								 bool is_default);
	Result<void> Remove(const LaunchProfileId& id);
	Result<std::vector<LaunchProfile>> List() const;
	Result<std::vector<LaunchProfile>> ListByGame(const GameId& game_id) const;
	Result<std::optional<LaunchProfile>> FindById(
		const LaunchProfileId& id) const;

private:
	Result<void> Validate(std::string_view name, const GameId& game_id,
						  const EmulatorId& emulator_id,
						  std::string_view strategy_id) const;

	LaunchProfileRepository& profiles_;
	GameRepository& games_;
	EmulatorRepository& emulators_;
	IdGenerator& ids_;
};

}
