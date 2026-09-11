#include "emu/launch/direct_strategy.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "test_support.h"

namespace emu {
namespace {

void TestInfo() {
	DirectLaunchStrategy strategy;
	const auto info = strategy.Info();
	testing::Check(info.id == "direct", "Strategy id changed");
	testing::Check(info.name == "Direct", "Strategy name changed");
	testing::Check(!info.description.empty(), "Strategy description is empty");
}

void TestRequest() {
	DirectLaunchStrategy strategy;
	const Game game{"Game", "games/game.rom", "console"};
	const Emulator emulator{"Emulator",
							"bin/emulator.exe",
							"console",
							"direct",
							{"--fullscreen", "two words", ""}};

	const auto request = strategy.BuildRequest({emulator, game});
	testing::Check(request.executable == emulator.path,
				   "Executable path changed");
	testing::Check(request.arguments ==
					   std::vector<std::string>{
						   game.path.string(), "--fullscreen", "two words", ""},
				   "Arguments or their order changed");
}

void TestEmptyValues() {
	DirectLaunchStrategy strategy;
	const Game empty_game{"Game", {}, "console"};
	const Emulator emulator{
		"Emulator", "emulator.exe", "console", "direct", {}};
	const auto request = strategy.BuildRequest({emulator, empty_game});
	testing::Check(request.arguments == std::vector<std::string>{""},
				   "Empty game path must remain the first argument");

	auto invalid = emulator;
	invalid.path.clear();
	testing::ExpectThrow<std::invalid_argument>(
		[&] { strategy.BuildRequest({invalid, empty_game}); },
		"Empty executable path must throw");
}

}
}

int main() {
	return emu::testing::RunTests([] {
		emu::TestInfo();
		emu::TestRequest();
		emu::TestEmptyValues();
	});
}
