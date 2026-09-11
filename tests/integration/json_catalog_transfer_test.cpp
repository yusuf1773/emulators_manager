#include "emu/persistence/json_catalog_transfer.h"

#include <gtest/gtest.h>

#include <fstream>

#include "emu/application/emulator_service.h"
#include "emu/application/game_service.h"
#include "emu/application/launch_profile_service.h"
#include "emu/persistence/sqlite_database.h"
#include "emu/persistence/sqlite_repositories.h"
#include "fakes.h"
#include "nlohmann/json.hpp"
#include "temporary_directory.h"

namespace emu {
namespace {

TEST(JsonCatalogTransferTest, ImportsLegacyFilesAndCreatesProfiles) {
	testing::TemporaryDirectory directory;
	const auto games_path = directory.path() / "games.json";
	const auto emulators_path = directory.path() / "emulators.json";
	std::ofstream(games_path) << nlohmann::json::array(
		{{{"name", "Game"}, {"path", "game.rom"}, {"platform", "console"}}});
	std::ofstream(emulators_path)
		<< nlohmann::json::array({{{"name", "Emulator"},
								   {"path", "emulator.exe"},
								   {"platform", "console"},
								   {"launch_strategy_id", "direct"},
								   {"args", {"--base"}}}});
	SqliteDatabase database(directory.path() / "catalog.db");
	SqliteGameRepository games(database);
	SqliteEmulatorRepository emulators(database);
	SqliteLaunchProfileRepository profiles(database);
	testing::FixedIdGenerator ids({"game", "emulator", "profile"});
	GameService game_service(games, ids);
	EmulatorService emulator_service(emulators, ids);
	LaunchProfileService profile_service(profiles, games, emulators, ids);
	JsonCatalogTransfer transfer(games, emulators, profiles, game_service,
								 emulator_service, profile_service);

	auto result = transfer.ImportLegacyIfEmpty(games_path, emulators_path);

	ASSERT_TRUE(result);
	ASSERT_TRUE(games.FindAll());
	ASSERT_EQ(games.FindAll()->size(), 1);
	EXPECT_EQ((*games.FindAll())[0].id, GameId("game"));
	ASSERT_TRUE(emulators.FindAll());
	EXPECT_EQ((*emulators.FindAll())[0].base_arguments,
			  (std::vector<std::string>{"--base"}));
	ASSERT_TRUE(profiles.FindAll());
	ASSERT_EQ(profiles.FindAll()->size(), 1);
	EXPECT_TRUE((*profiles.FindAll())[0].is_default);
}

TEST(JsonCatalogTransferTest, ExportsCurrentSchemaAndDoesNotReimport) {
	testing::TemporaryDirectory directory;
	SqliteDatabase database(directory.path() / "catalog.db");
	SqliteGameRepository games(database);
	SqliteEmulatorRepository emulators(database);
	SqliteLaunchProfileRepository profiles(database);
	testing::FixedIdGenerator ids({"unused"});
	GameService game_service(games, ids);
	EmulatorService emulator_service(emulators, ids);
	LaunchProfileService profile_service(profiles, games, emulators, ids);
	JsonCatalogTransfer transfer(games, emulators, profiles, game_service,
								 emulator_service, profile_service);
	ASSERT_TRUE(games.Save({GameId("game"), "Game", "game.rom", "console"}));
	const auto legacy_path = directory.path() / "legacy.json";
	std::ofstream(legacy_path) << nlohmann::json::array(
		{{{"name", "Other"}, {"path", "other.rom"}, {"platform", "console"}}});
	ASSERT_TRUE(transfer.ImportLegacyIfEmpty(legacy_path, {}));
	ASSERT_EQ(games.FindAll()->size(), 1);

	const auto export_path = directory.path() / "export" / "catalog.json";
	ASSERT_TRUE(transfer.Export(export_path));
	std::ifstream input(export_path);
	const auto json = nlohmann::json::parse(input);
	EXPECT_EQ(json.at("schema_version"), 1);
	ASSERT_EQ(json.at("games").size(), 1);
	EXPECT_EQ(json.at("games")[0].at("id"), "game");
	EXPECT_TRUE(json.at("emulators").empty());
	EXPECT_TRUE(json.at("launch_profiles").empty());
}

}
}
