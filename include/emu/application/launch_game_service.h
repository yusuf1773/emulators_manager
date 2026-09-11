#pragma once

#include <string>

#include "emu/application/launch_strategy_registry.h"
#include "emu/application/process_runner.h"
#include "emu/application/repository.h"
#include "emu/domain/emulator.h"
#include "emu/domain/game.h"

namespace emu {

class LaunchGameService {
	public:
	LaunchGameService(const Repository<Game>& games,
					  const Repository<Emulator>& emulators,
					  const LaunchStrategyRegistry& strategies,
					  ProcessRunner& runner);

	void Launch(const std::string& game_name,
				const std::string& emulator_name) const;

	private:
	const Repository<Game>& games_;
	const Repository<Emulator>& emulators_;
	const LaunchStrategyRegistry& strategies_;
	ProcessRunner& runner_;
};

}
