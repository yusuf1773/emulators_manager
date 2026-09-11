#include "emu/persistence/json_catalog_transfer.h"

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"

namespace emu {
namespace {

struct LegacyGame {
	std::string name;
	std::filesystem::path path;
	std::string platform;
};

struct LegacyEmulator {
	std::string name;
	std::filesystem::path path;
	std::string platform;
	std::string launch_strategy_id;
	std::vector<std::string> args;
};

void from_json(const nlohmann::json& json, LegacyGame& game) {
	json.at("name").get_to(game.name);
	json.at("path").get_to(game.path);
	json.at("platform").get_to(game.platform);
}

void from_json(const nlohmann::json& json, LegacyEmulator& emulator) {
	json.at("name").get_to(emulator.name);
	json.at("path").get_to(emulator.path);
	json.at("platform").get_to(emulator.platform);
	json.at("launch_strategy_id").get_to(emulator.launch_strategy_id);
	json.at("args").get_to(emulator.args);
}

nlohmann::json ToJson(const Game& game) {
	return {{"id", game.id.value()},
			{"name", game.name},
			{"content_path", game.content_path},
			{"platform", game.platform}};
}

nlohmann::json ToJson(const Emulator& emulator) {
	return {{"id", emulator.id.value()},
			{"name", emulator.name},
			{"executable_path", emulator.executable_path},
			{"platform", emulator.platform},
			{"base_arguments", emulator.base_arguments}};
}

nlohmann::json ToJson(const LaunchProfile& profile) {
	return {{"id", profile.id.value()},
			{"name", profile.name},
			{"game_id", profile.game_id.value()},
			{"emulator_id", profile.emulator_id.value()},
			{"strategy_id", profile.strategy_id},
			{"arguments", profile.arguments},
			{"working_directory", profile.working_directory},
			{"is_default", profile.is_default}};
}

}

JsonCatalogTransfer::JsonCatalogTransfer(GameRepository& games,
										 EmulatorRepository& emulators,
										 LaunchProfileRepository& profiles,
										 GameService& game_service,
										 EmulatorService& emulator_service,
										 LaunchProfileService& profile_service)
	: games_(games),
	  emulators_(emulators),
	  profiles_(profiles),
	  game_service_(game_service),
	  emulator_service_(emulator_service),
	  profile_service_(profile_service) {}

Result<void> JsonCatalogTransfer::ImportLegacyIfEmpty(
	const std::filesystem::path& games_path,
	const std::filesystem::path& emulators_path) {
	auto games = games_.FindAll();
	if (!games) return std::unexpected(games.error());
	auto emulators = emulators_.FindAll();
	if (!emulators) return std::unexpected(emulators.error());
	if (!games->empty() || !emulators->empty()) return {};
	if (!std::filesystem::exists(games_path) &&
		!std::filesystem::exists(emulators_path)) {
		return {};
	}

	try {
		std::vector<Game> imported_games;
		std::vector<std::pair<Emulator, std::string>> imported_emulators;
		if (std::filesystem::exists(games_path)) {
			std::ifstream input(games_path);
			for (auto& legacy :
				 nlohmann::json::parse(input).get<std::vector<LegacyGame>>()) {
				auto created = game_service_.Create(std::move(legacy.name),
													std::move(legacy.path),
													std::move(legacy.platform));
				if (!created) return std::unexpected(created.error());
				imported_games.push_back(std::move(*created));
			}
		}
		if (std::filesystem::exists(emulators_path)) {
			std::ifstream input(emulators_path);
			for (auto& legacy : nlohmann::json::parse(input)
									.get<std::vector<LegacyEmulator>>()) {
				const std::string strategy =
					legacy.launch_strategy_id.empty()
						? "direct"
						: std::move(legacy.launch_strategy_id);
				auto created = emulator_service_.Create(
					std::move(legacy.name), std::move(legacy.path),
					std::move(legacy.platform), std::move(legacy.args));
				if (!created) return std::unexpected(created.error());
				imported_emulators.emplace_back(std::move(*created), strategy);
			}
		}
		for (const auto& game : imported_games) {
			bool first = true;
			for (const auto& [emulator, strategy] : imported_emulators) {
				if (game.platform != emulator.platform) continue;
				auto created =
					profile_service_.Create(emulator.name, game.id, emulator.id,
											strategy, {}, {}, first);
				if (!created) return std::unexpected(created.error());
				first = false;
			}
		}
		return {};
	} catch (const std::exception& error) {
		return Failure(ErrorCode::kPersistence, error.what());
	}
}

Result<void> JsonCatalogTransfer::Export(
	const std::filesystem::path& path) const {
	auto games = games_.FindAll();
	if (!games) return std::unexpected(games.error());
	auto emulators = emulators_.FindAll();
	if (!emulators) return std::unexpected(emulators.error());
	auto profiles = profiles_.FindAll();
	if (!profiles) return std::unexpected(profiles.error());
	try {
		if (!path.parent_path().empty()) {
			std::filesystem::create_directories(path.parent_path());
		}
		nlohmann::json json{{"schema_version", 1},
							{"games", nlohmann::json::array()},
							{"emulators", nlohmann::json::array()},
							{"launch_profiles", nlohmann::json::array()}};
		for (const auto& game : *games) json["games"].push_back(ToJson(game));
		for (const auto& emulator : *emulators) {
			json["emulators"].push_back(ToJson(emulator));
		}
		for (const auto& profile : *profiles) {
			json["launch_profiles"].push_back(ToJson(profile));
		}
		std::ofstream output(path, std::ios::trunc);
		output.exceptions(std::ios::failbit | std::ios::badbit);
		output << json.dump(4);
		return {};
	} catch (const std::exception& error) {
		return Failure(ErrorCode::kPersistence, error.what());
	}
}

}
