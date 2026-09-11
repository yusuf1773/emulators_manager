#pragma once

#include "emu/application/emulator_repository.h"
#include "emu/application/game_repository.h"
#include "emu/application/launch_profile_repository.h"
#include "emu/application/launch_strategy_registry.h"
#include "emu/application/process_runner.h"
#include "emu/domain/entity_ids.h"

namespace emu {

class LaunchGameService {
public:
	LaunchGameService(GameRepository& games, EmulatorRepository& emulators,
					  LaunchProfileRepository& profiles,
					  const LaunchStrategyRegistry& strategies,
					  ProcessRunner& runner);

	Result<ProcessResult> Launch(const LaunchProfileId& profile_id) const;

private:
	GameRepository& games_;
	EmulatorRepository& emulators_;
	LaunchProfileRepository& profiles_;
	const LaunchStrategyRegistry& strategies_;
	ProcessRunner& runner_;
};

}
