#include "emu/application/launch_profile_service.h"

#include <gtest/gtest.h>

#include "fakes.h"

namespace emu {
namespace {

class LaunchProfileServiceTest : public ::testing::Test {
protected:
	LaunchProfileServiceTest()
		: ids({"profile-1", "profile-2", "profile-3"}),
		  service(profiles, games, emulators, ids) {
		games.values.push_back({GameId("game"), "Game", "game.rom", "console"});
		emulators.values.push_back({EmulatorId("emulator"),
									"Emulator",
									"emulator.exe",
									"console",
									{}});
	}

	testing::MemoryLaunchProfileRepository profiles;
	testing::MemoryGameRepository games;
	testing::MemoryEmulatorRepository emulators;
	testing::FixedIdGenerator ids;
	LaunchProfileService service;
};

TEST_F(LaunchProfileServiceTest, CreatesProfileWithReferences) {
	auto result =
		service.Create("Default", GameId("game"), EmulatorId("emulator"),
					   "direct", {"--flag"}, "working", true);
	ASSERT_TRUE(result);
	EXPECT_EQ(result->id, LaunchProfileId("profile-1"));
	EXPECT_TRUE(result->is_default);
	EXPECT_EQ(profiles.values, std::vector<LaunchProfile>{*result});
}

TEST_F(LaunchProfileServiceTest, RejectsMissingReferences) {
	auto missing_game =
		service.Create("Profile", GameId("missing"), EmulatorId("emulator"),
					   "direct", {}, {}, false);
	auto missing_emulator =
		service.Create("Profile", GameId("game"), EmulatorId("missing"),
					   "direct", {}, {}, false);
	ASSERT_FALSE(missing_game);
	EXPECT_EQ(missing_game.error().code, ErrorCode::kNotFound);
	ASSERT_FALSE(missing_emulator);
	EXPECT_EQ(missing_emulator.error().code, ErrorCode::kNotFound);
}

TEST_F(LaunchProfileServiceTest, KeepsOneDefaultProfilePerGame) {
	ASSERT_TRUE(service.Create("First", GameId("game"), EmulatorId("emulator"),
							   "direct", {}, {}, true));
	ASSERT_TRUE(service.Create("Second", GameId("game"), EmulatorId("emulator"),
							   "direct", {}, {}, true));
	ASSERT_EQ(profiles.values.size(), 2);
	EXPECT_FALSE(profiles.values[0].is_default);
	EXPECT_TRUE(profiles.values[1].is_default);
}

TEST_F(LaunchProfileServiceTest, RejectsDuplicateNameForSameGame) {
	ASSERT_TRUE(service.Create("Profile", GameId("game"),
							   EmulatorId("emulator"), "direct", {}, {},
							   false));
	auto duplicate =
		service.Create("Profile", GameId("game"), EmulatorId("emulator"),
					   "direct", {}, {}, false);
	ASSERT_FALSE(duplicate);
	EXPECT_EQ(duplicate.error().code, ErrorCode::kDuplicateName);
}

}
}
