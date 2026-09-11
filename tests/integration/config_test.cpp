#include "emu/persistence/config.h"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "emu/persistence/json_serialization.h"
#include "test_support.h"

namespace emu {
namespace {

std::string ReadText(const std::filesystem::path& path) {
	std::ifstream input(path);
	return {std::istreambuf_iterator<char>(input),
			std::istreambuf_iterator<char>()};
}

void TestDefaultConfig() {
	testing::TemporaryDirectory directory;
	const auto path = directory.path() / "nested" / "config.json";
	const Config config(path);

	testing::Check(std::filesystem::is_regular_file(path),
				   "Default config file was not created");
	testing::Check(
		config.GetData().games_path == path.parent_path() / "games.json",
		"Default games path was resolved incorrectly");
	testing::Check(config.GetData().emulators_path ==
					   path.parent_path() / "emulators.json",
				   "Default emulators path was resolved incorrectly");
	testing::Check(
		config.GetData().history_path == path.parent_path() / "history.json",
		"Default history path was resolved incorrectly");
	testing::Check(config.GetData().history_max_size == 15,
				   "Default history limit changed");

	const auto json = nlohmann::json::parse(ReadText(path));
	testing::Check(json.size() == 4, "Default config has unexpected fields");
}

void TestConfiguredPaths() {
	testing::TemporaryDirectory directory;
	const auto path = directory.path() / "config.json";
	ConfigData data;
	data.games_path = directory.path() / "absolute-games.json";
	data.emulators_path = "data/emulators.json";
	data.history_path = "history/custom.json";
	data.history_max_size = 42;
	std::ofstream(path) << nlohmann::json(data);

	const Config config(path);
	testing::Check(config.GetData().games_path == data.games_path,
				   "Absolute path changed");
	testing::Check(config.GetData().emulators_path ==
					   path.parent_path() / data.emulators_path,
				   "Relative emulator path was resolved incorrectly");
	testing::Check(
		config.GetData().history_path == path.parent_path() / data.history_path,
		"Relative history path was resolved incorrectly");
	testing::Check(config.GetData().history_max_size == 42,
				   "Configured history limit changed");
}

void TestInvalidConfigIsPreserved() {
	testing::TemporaryDirectory directory;
	const auto malformed = directory.path() / "malformed.json";
	std::ofstream(malformed) << "{broken";
	const auto before = ReadText(malformed);
	testing::ExpectThrow<std::runtime_error>([&] { Config config(malformed); },
											 "Malformed config must throw");
	testing::Check(ReadText(malformed) == before,
				   "Malformed config was overwritten");

	const auto incomplete = directory.path() / "incomplete.json";
	std::ofstream(incomplete) << R"({"games_path":"games.json"})";
	testing::ExpectThrow<std::runtime_error>([&] { Config config(incomplete); },
											 "Incomplete config must throw");
}

}
}

int main() {
	return emu::testing::RunTests([] {
		emu::TestDefaultConfig();
		emu::TestConfiguredPaths();
		emu::TestInvalidConfigIsPreserved();
	});
}
