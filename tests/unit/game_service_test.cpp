#include "emu/application/game_service.h"

#include <gtest/gtest.h>

#include "fakes.h"

namespace emu {
namespace {

TEST(GameServiceTest, CreatesGameWithGeneratedId) {
	testing::MemoryGameRepository repository;
	testing::FixedIdGenerator ids({"game-1"});
	GameService service(repository, ids);
	auto result = service.Create("Game", "game.rom", "console");
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result->id, GameId("game-1"));
	EXPECT_EQ(repository.values, std::vector<Game>{*result});
}

TEST(GameServiceTest, RejectsInvalidAndDuplicateGames) {
	testing::MemoryGameRepository repository;
	repository.values.push_back(
		{GameId("existing"), "Game", "game.rom", "console"});
	testing::FixedIdGenerator ids({"unused"});
	GameService service(repository, ids);
	auto empty_name = service.Create("", "game.rom", "console");
	auto empty_path = service.Create("Other", {}, "console");
	auto duplicate = service.Create("Game", "other.rom", "console");
	ASSERT_FALSE(empty_name);
	EXPECT_EQ(empty_name.error().code, ErrorCode::kInvalidArgument);
	ASSERT_FALSE(empty_path);
	EXPECT_EQ(empty_path.error().code, ErrorCode::kInvalidArgument);
	ASSERT_FALSE(duplicate);
	EXPECT_EQ(duplicate.error().code, ErrorCode::kDuplicateName);
	EXPECT_EQ(repository.values.size(), 1);
}

TEST(GameServiceTest, PreservesIdDuringUpdate) {
	testing::MemoryGameRepository repository;
	repository.values.push_back(
		{GameId("game-1"), "Old", "old.rom", "console"});
	testing::FixedIdGenerator ids({});
	GameService service(repository, ids);
	auto result = service.Update(GameId("game-1"), "New", "new.rom", "other");
	ASSERT_TRUE(result);
	EXPECT_EQ(result->id, GameId("game-1"));
	EXPECT_EQ(repository.values[0].name, "New");
}

TEST(GameServiceTest, PropagatesPersistenceError) {
	testing::MemoryGameRepository repository;
	repository.save_error = true;
	testing::FixedIdGenerator ids({"game-1"});
	GameService service(repository, ids);
	auto result = service.Create("Game", "game.rom", "console");
	ASSERT_FALSE(result);
	EXPECT_EQ(result.error().code, ErrorCode::kPersistence);
}

}
}
