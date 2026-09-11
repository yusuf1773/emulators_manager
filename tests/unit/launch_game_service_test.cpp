#include "emu/application/launch_game_service.h"

#include <gtest/gtest.h>

#include "emu/launch/direct_strategy.h"
#include "fakes.h"

namespace emu {
namespace {

class LaunchGameServiceTest : public ::testing::Test {
protected:
	LaunchGameServiceTest()
		: service(games, emulators, profiles, strategies, runner) {
		games.values.push_back({GameId("game"), "Game", "game.rom", "console"});
		emulators.values.push_back({EmulatorId("emulator"),
									"Emulator",
									"emulator.exe",
									"console",
									{"--base"}});
		profiles.values.push_back({LaunchProfileId("profile"),
								   "Profile",
								   GameId("game"),
								   EmulatorId("emulator"),
								   "direct",
								   {"--profile"},
								   "working",
								   true});
		strategies.Add<DirectLaunchStrategy>();
	}

	testing::MemoryGameRepository games;
	testing::MemoryEmulatorRepository emulators;
	testing::MemoryLaunchProfileRepository profiles;
	LaunchStrategyRegistry strategies;
	testing::RecordingProcessRunner runner;
	LaunchGameService service;
};

TEST_F(LaunchGameServiceTest, RunsResolvedProfile) {
	auto result = service.Launch(LaunchProfileId("profile"));
	ASSERT_TRUE(result);
	EXPECT_EQ(result->exit_code, 0);
	ASSERT_EQ(runner.requests.size(), 1);
	EXPECT_EQ(runner.requests[0].arguments,
			  (std::vector<std::string>{"game.rom", "--base", "--profile"}));
	EXPECT_EQ(runner.requests[0].working_directory, "working");
}

TEST_F(LaunchGameServiceTest, RejectsMissingEntitiesBeforeStartingProcess) {
	auto missing_profile = service.Launch(LaunchProfileId("missing"));
	ASSERT_FALSE(missing_profile);
	EXPECT_EQ(missing_profile.error().code, ErrorCode::kNotFound);
	profiles.values[0].game_id = GameId("missing");
	auto missing_game = service.Launch(LaunchProfileId("profile"));
	ASSERT_FALSE(missing_game);
	EXPECT_EQ(missing_game.error().code, ErrorCode::kNotFound);
	EXPECT_TRUE(runner.requests.empty());
}

TEST_F(LaunchGameServiceTest, PropagatesProcessFailure) {
	runner.error = true;
	auto result = service.Launch(LaunchProfileId("profile"));
	ASSERT_FALSE(result);
	EXPECT_EQ(result.error().code, ErrorCode::kProcess);
}

TEST_F(LaunchGameServiceTest, ReportsUnknownStrategy) {
	profiles.values[0].strategy_id = "missing";
	auto result = service.Launch(LaunchProfileId("profile"));
	ASSERT_FALSE(result);
	EXPECT_EQ(result.error().code, ErrorCode::kInvalidArgument);
	EXPECT_TRUE(runner.requests.empty());
}

}
}
