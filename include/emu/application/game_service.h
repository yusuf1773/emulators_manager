#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "emu/application/game_repository.h"
#include "emu/application/id_generator.h"

namespace emu {

class GameService {
public:
	GameService(GameRepository& games, IdGenerator& ids);

	Result<Game> Create(std::string name, std::filesystem::path content_path,
						std::string platform);
	Result<Game> Update(GameId id, std::string name,
						std::filesystem::path content_path,
						std::string platform);
	Result<void> Remove(const GameId& id);
	Result<std::vector<Game>> List() const;
	Result<std::optional<Game>> FindById(const GameId& id) const;
	Result<std::optional<Game>> FindByName(std::string_view name) const;

private:
	GameRepository& games_;
	IdGenerator& ids_;
};

}
