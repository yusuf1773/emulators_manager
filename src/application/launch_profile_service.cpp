#include "emu/application/launch_profile_service.h"

#include <utility>

namespace emu {

LaunchProfileService::LaunchProfileService(LaunchProfileRepository& profiles,
										   GameRepository& games,
										   EmulatorRepository& emulators,
										   IdGenerator& ids)
	: profiles_(profiles), games_(games), emulators_(emulators), ids_(ids) {}

Result<void> LaunchProfileService::Validate(
	std::string_view name, const GameId& game_id, const EmulatorId& emulator_id,
	std::string_view strategy_id) const {
	if (name.empty() || game_id.empty() || emulator_id.empty() ||
		strategy_id.empty()) {
		return Failure(ErrorCode::kInvalidArgument,
					   "Launch profile has empty required fields");
	}
	auto game = games_.FindById(game_id);
	if (!game) return std::unexpected(game.error());
	if (!*game) return Failure(ErrorCode::kNotFound, "Game not found");
	auto emulator = emulators_.FindById(emulator_id);
	if (!emulator) return std::unexpected(emulator.error());
	if (!*emulator) {
		return Failure(ErrorCode::kNotFound, "Emulator not found");
	}
	return {};
}

Result<LaunchProfile> LaunchProfileService::Create(
	std::string name, GameId game_id, EmulatorId emulator_id,
	std::string strategy_id, std::vector<std::string> arguments,
	std::filesystem::path working_directory, bool is_default) {
	if (auto validation = Validate(name, game_id, emulator_id, strategy_id);
		!validation) {
		return std::unexpected(validation.error());
	}
	auto duplicate = profiles_.FindByName(game_id, name);
	if (!duplicate) return std::unexpected(duplicate.error());
	if (*duplicate) {
		return Failure(ErrorCode::kDuplicateName,
					   "Launch profile name already exists for this game");
	}
	LaunchProfile profile{LaunchProfileId(ids_.Next()), std::move(name),
						  std::move(game_id),			std::move(emulator_id),
						  std::move(strategy_id),		std::move(arguments),
						  std::move(working_directory), is_default};
	if (auto saved = profiles_.Save(profile); !saved) {
		return std::unexpected(saved.error());
	}
	return profile;
}

Result<LaunchProfile> LaunchProfileService::Update(
	LaunchProfileId id, std::string name, GameId game_id,
	EmulatorId emulator_id, std::string strategy_id,
	std::vector<std::string> arguments, std::filesystem::path working_directory,
	bool is_default) {
	auto current = profiles_.FindById(id);
	if (!current) return std::unexpected(current.error());
	if (!*current) {
		return Failure(ErrorCode::kNotFound, "Launch profile not found");
	}
	if (auto validation = Validate(name, game_id, emulator_id, strategy_id);
		!validation) {
		return std::unexpected(validation.error());
	}
	auto duplicate = profiles_.FindByName(game_id, name);
	if (!duplicate) return std::unexpected(duplicate.error());
	if (*duplicate && (*duplicate)->id != id) {
		return Failure(ErrorCode::kDuplicateName,
					   "Launch profile name already exists for this game");
	}
	LaunchProfile profile{std::move(id),
						  std::move(name),
						  std::move(game_id),
						  std::move(emulator_id),
						  std::move(strategy_id),
						  std::move(arguments),
						  std::move(working_directory),
						  is_default};
	if (auto saved = profiles_.Save(profile); !saved) {
		return std::unexpected(saved.error());
	}
	return profile;
}

Result<void> LaunchProfileService::Remove(const LaunchProfileId& id) {
	return profiles_.Remove(id);
}

Result<std::vector<LaunchProfile>> LaunchProfileService::List() const {
	return profiles_.FindAll();
}

Result<std::vector<LaunchProfile>> LaunchProfileService::ListByGame(
	const GameId& game_id) const {
	return profiles_.FindByGame(game_id);
}

Result<std::optional<LaunchProfile>> LaunchProfileService::FindById(
	const LaunchProfileId& id) const {
	return profiles_.FindById(id);
}

}
