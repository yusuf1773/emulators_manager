#include "emu/application/launch_game_service.h"

#include <stdexcept>

namespace emu {

LaunchGameService::LaunchGameService(GameRepository& games,
									 EmulatorRepository& emulators,
									 LaunchProfileRepository& profiles,
									 const LaunchStrategyRegistry& strategies,
									 ProcessRunner& runner)
	: games_(games),
	  emulators_(emulators),
	  profiles_(profiles),
	  strategies_(strategies),
	  runner_(runner) {}

Result<ProcessResult> LaunchGameService::Launch(
	const LaunchProfileId& profile_id) const {
	auto profile = profiles_.FindById(profile_id);
	if (!profile) return std::unexpected(profile.error());
	if (!*profile) {
		return Failure(ErrorCode::kNotFound, "Launch profile not found");
	}
	auto game = games_.FindById((*profile)->game_id);
	if (!game) return std::unexpected(game.error());
	if (!*game) return Failure(ErrorCode::kNotFound, "Game not found");
	auto emulator = emulators_.FindById((*profile)->emulator_id);
	if (!emulator) return std::unexpected(emulator.error());
	if (!*emulator) {
		return Failure(ErrorCode::kNotFound, "Emulator not found");
	}

	const LaunchStrategy* strategy = nullptr;
	try {
		strategy = &strategies_.GetById((*profile)->strategy_id);
	} catch (const std::invalid_argument&) {
		return Failure(ErrorCode::kInvalidArgument, "Unknown launch strategy");
	}
	auto request = strategy->BuildRequest({**emulator, **game, **profile});
	if (!request) return std::unexpected(request.error());
	return runner_.Run(*request);
}

}
