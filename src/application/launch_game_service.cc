#include "emu/application/launch_game_service.h"

#include <string>

namespace emu {

LaunchGameService::LaunchGameService(const Repository<Game>& games,
                                     const Repository<Emulator>& emulators,
                                     const LaunchStrategyRegistry& strategies,
                                     ProcessRunner& runner)
    : games_(games),
      emulators_(emulators),
      strategies_(strategies),
      runner_(runner) {}

void LaunchGameService::Launch(const std::string& game_name,
                               const std::string& emulator_name) const {
  const auto& game = games_.GetByName(game_name);
  const auto& emulator = emulators_.GetByName(emulator_name);
  const auto& strategy = strategies_.GetById(emulator.launch_strategy_id);
  runner_.Run(strategy.BuildRequest({emulator, game}));
}

}
