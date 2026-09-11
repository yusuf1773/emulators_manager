#include "emu/launch/direct_strategy.h"

#include <gtest/gtest.h>

namespace emu {
namespace {

TEST(DirectStrategyTest, BuildsArgumentsInExpectedOrder) {
	const Game game{GameId("game"), "Game", "game.rom", "console"};
	const Emulator emulator{EmulatorId("emulator"),
							"Emulator",
							"emulator.exe",
							"console",
							{"--base"}};
	const LaunchProfile profile{LaunchProfileId("profile"),
								"Profile",
								game.id,
								emulator.id,
								"direct",
								{"--profile"},
								"working",
								true};
	DirectLaunchStrategy strategy;
	auto result = strategy.BuildRequest({emulator, game, profile});
	ASSERT_TRUE(result);
	EXPECT_EQ(result->executable, emulator.executable_path);
	EXPECT_EQ(result->arguments,
			  (std::vector<std::string>{"game.rom", "--base", "--profile"}));
	EXPECT_EQ(result->working_directory, "working");
}

TEST(DirectStrategyTest, RejectsEmptyExecutable) {
	const Game game{GameId("game"), "Game", "game.rom", "console"};
	const Emulator emulator{
		EmulatorId("emulator"), "Emulator", {}, "console", {}};
	const LaunchProfile profile{LaunchProfileId("profile"),
								"Profile",
								game.id,
								emulator.id,
								"direct",
								{},
								{},
								false};
	DirectLaunchStrategy strategy;
	auto result = strategy.BuildRequest({emulator, game, profile});
	ASSERT_FALSE(result);
	EXPECT_EQ(result.error().code, ErrorCode::kInvalidArgument);
}

}
}
