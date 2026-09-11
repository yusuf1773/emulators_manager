#include "emu/application/game_service.h"

#include <utility>

namespace emu {
namespace {

Result<void> ValidateGame(std::string_view name,
						  const std::filesystem::path& content_path) {
	if (name.empty()) {
		return Failure(ErrorCode::kInvalidArgument, "Game name is empty");
	}
	if (content_path.empty()) {
		return Failure(ErrorCode::kInvalidArgument, "Game path is empty");
	}
	return {};
}

}

GameService::GameService(GameRepository& games, IdGenerator& ids)
	: games_(games), ids_(ids) {}

Result<Game> GameService::Create(std::string name,
								 std::filesystem::path content_path,
								 std::string platform) {
	if (auto validation = ValidateGame(name, content_path); !validation) {
		return std::unexpected(validation.error());
	}
	auto existing = games_.FindByName(name);
	if (!existing) return std::unexpected(existing.error());
	if (*existing) {
		return Failure(ErrorCode::kDuplicateName, "Game name already exists");
	}
	Game game{GameId(ids_.Next()), std::move(name), std::move(content_path),
			  std::move(platform)};
	if (auto saved = games_.Save(game); !saved) {
		return std::unexpected(saved.error());
	}
	return game;
}

Result<Game> GameService::Update(GameId id, std::string name,
								 std::filesystem::path content_path,
								 std::string platform) {
	if (auto validation = ValidateGame(name, content_path); !validation) {
		return std::unexpected(validation.error());
	}
	auto current = games_.FindById(id);
	if (!current) return std::unexpected(current.error());
	if (!*current) return Failure(ErrorCode::kNotFound, "Game not found");
	auto duplicate = games_.FindByName(name);
	if (!duplicate) return std::unexpected(duplicate.error());
	if (*duplicate && (*duplicate)->id != id) {
		return Failure(ErrorCode::kDuplicateName, "Game name already exists");
	}
	Game game{std::move(id), std::move(name), std::move(content_path),
			  std::move(platform)};
	if (auto saved = games_.Save(game); !saved) {
		return std::unexpected(saved.error());
	}
	return game;
}

Result<void> GameService::Remove(const GameId& id) { return games_.Remove(id); }

Result<std::vector<Game>> GameService::List() const { return games_.FindAll(); }

Result<std::optional<Game>> GameService::FindById(const GameId& id) const {
	return games_.FindById(id);
}

Result<std::optional<Game>> GameService::FindByName(
	std::string_view name) const {
	return games_.FindByName(name);
}

}
