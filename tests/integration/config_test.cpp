#include "emu/persistence/config.h"

#include <gtest/gtest.h>

#include <fstream>

#include "nlohmann/json.hpp"
#include "temporary_directory.h"

namespace emu {
namespace {

TEST(ConfigTest, CreatesDefaultsAndResolvesPaths) {
	testing::TemporaryDirectory directory;
	const auto path = directory.path() / "config" / "config.json";
	const Config config(path);
	EXPECT_TRUE(std::filesystem::is_regular_file(path));
	EXPECT_EQ(config.GetData().database_path,
			  path.parent_path() / "emulators.db");
	EXPECT_EQ(config.GetData().games_path, path.parent_path() / "games.json");
	EXPECT_EQ(config.GetData().export_path,
			  path.parent_path() / "catalog.json");
	EXPECT_EQ(config.GetData().history_max_size, 15);
}

TEST(ConfigTest, LoadsOldConfigWithoutNewFields) {
	testing::TemporaryDirectory directory;
	const auto path = directory.path() / "config.json";
	std::ofstream(path) << nlohmann::json{
		{"games_path", "old-games.json"},
		{"emulators_path", "old-emulators.json"},
		{"history_path", "old-history.json"},
		{"history_max_size", 4}};
	const Config config(path);
	EXPECT_EQ(config.GetData().database_path,
			  directory.path() / "emulators.db");
	EXPECT_EQ(config.GetData().games_path, directory.path() / "old-games.json");
	EXPECT_EQ(config.GetData().history_max_size, 4);
}

TEST(ConfigTest, RejectsMalformedConfig) {
	testing::TemporaryDirectory directory;
	const auto path = directory.path() / "config.json";
	std::ofstream(path) << "{broken";
	EXPECT_THROW(Config config(path), std::runtime_error);
}

}
}
