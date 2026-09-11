#include "emu/persistence/sqlite_repositories.h"

#include <gtest/gtest.h>
#include <sqlite3.h>

#include <chrono>

#include "emu/persistence/sqlite_database.h"
#include "temporary_directory.h"

namespace emu {
namespace {

class SqliteRepositoriesTest : public ::testing::Test {
protected:
	SqliteRepositoriesTest()
		: database(directory.path() / "data" / "catalog.db"),
		  games(database),
		  emulators(database),
		  profiles(database),
		  history(database) {}

	testing::TemporaryDirectory directory;
	SqliteDatabase database;
	SqliteGameRepository games;
	SqliteEmulatorRepository emulators;
	SqliteLaunchProfileRepository profiles;
	SqliteHistoryRepository history;
};

TEST_F(SqliteRepositoriesTest, PersistsAllCatalogEntities) {
	const Game game{GameId("game"), "Game", "game.rom", "console"};
	const Emulator emulator{EmulatorId("emulator"),
							"Emulator",
							"emulator.exe",
							"console",
							{"--base", "two words"}};
	const LaunchProfile profile{LaunchProfileId("profile"),
								"Default",
								game.id,
								emulator.id,
								"direct",
								{"--profile"},
								"working",
								true};
	ASSERT_TRUE(games.Save(game));
	ASSERT_TRUE(emulators.Save(emulator));
	ASSERT_TRUE(profiles.Save(profile));
	ASSERT_TRUE(games.FindById(game.id));
	ASSERT_TRUE(*games.FindById(game.id));
	EXPECT_EQ(**games.FindById(game.id), game);
	EXPECT_EQ(**emulators.FindById(emulator.id), emulator);
	EXPECT_EQ(**profiles.FindById(profile.id), profile);
}

TEST_F(SqliteRepositoriesTest, EnforcesNamesAndReferences) {
	ASSERT_TRUE(games.Save({GameId("one"), "Game", "one.rom", "console"}));
	auto duplicate = games.Save({GameId("two"), "game", "two.rom", "console"});
	ASSERT_FALSE(duplicate);
	EXPECT_EQ(duplicate.error().code, ErrorCode::kPersistence);
	auto invalid_profile = profiles.Save({LaunchProfileId("profile"),
										  "Profile",
										  GameId("missing"),
										  EmulatorId("missing"),
										  "direct",
										  {},
										  {},
										  false});
	ASSERT_FALSE(invalid_profile);
	EXPECT_EQ(invalid_profile.error().code, ErrorCode::kPersistence);
}

TEST_F(SqliteRepositoriesTest, KeepsOneDefaultAndCascadesDeletes) {
	const Game game{GameId("game"), "Game", "game.rom", "console"};
	const Emulator emulator{
		EmulatorId("emulator"), "Emulator", "emulator.exe", "console", {}};
	ASSERT_TRUE(games.Save(game));
	ASSERT_TRUE(emulators.Save(emulator));
	ASSERT_TRUE(profiles.Save({LaunchProfileId("one"),
							   "One",
							   game.id,
							   emulator.id,
							   "direct",
							   {},
							   {},
							   true}));
	ASSERT_TRUE(profiles.Save({LaunchProfileId("two"),
							   "Two",
							   game.id,
							   emulator.id,
							   "direct",
							   {},
							   {},
							   true}));
	auto values = profiles.FindByGame(game.id);
	ASSERT_TRUE(values);
	ASSERT_EQ(values->size(), 2);
	EXPECT_TRUE((*values)[0].is_default);
	EXPECT_EQ((*values)[0].id, LaunchProfileId("two"));
	EXPECT_FALSE((*values)[1].is_default);
	ASSERT_TRUE(games.Remove(game.id));
	ASSERT_TRUE(profiles.FindAll());
	EXPECT_TRUE(profiles.FindAll()->empty());
}

TEST_F(SqliteRepositoriesTest, StoresAndTrimsHistory) {
	const auto time =
		std::chrono::system_clock::time_point(std::chrono::seconds(123));
	ASSERT_TRUE(history.Add({"one", time}));
	ASSERT_TRUE(history.Add({"two", time}));
	ASSERT_TRUE(history.Add({"three", time}));
	ASSERT_TRUE(history.Trim(2));
	auto values = history.FindRecent(10);
	ASSERT_TRUE(values);
	ASSERT_EQ(values->size(), 2);
	EXPECT_EQ((*values)[0].command, "three");
	EXPECT_EQ((*values)[1].command, "two");
	EXPECT_EQ((*values)[0].timestamp, time);
}

TEST(SqliteDatabaseTest, RejectsNewerSchemaVersion) {
	testing::TemporaryDirectory directory;
	const auto path = directory.path() / "future.db";
	sqlite3* database = nullptr;
	ASSERT_EQ(sqlite3_open(path.string().c_str(), &database), SQLITE_OK);
	ASSERT_EQ(sqlite3_exec(database, "PRAGMA user_version = 99", nullptr,
						   nullptr, nullptr),
			  SQLITE_OK);
	sqlite3_close(database);
	EXPECT_THROW(SqliteDatabase current(path), std::runtime_error);
}

}
}
