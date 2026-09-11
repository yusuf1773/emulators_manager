#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "emu/application/application_error.h"
#include "emu/domain/game.h"

namespace emu {

class GameRepository {
public:
	virtual ~GameRepository() = default;
	virtual Result<std::vector<Game>> FindAll() const = 0;
	virtual Result<std::optional<Game>> FindById(const GameId& id) const = 0;
	virtual Result<std::optional<Game>> FindByName(
		std::string_view name) const = 0;
	virtual Result<void> Save(const Game& game) = 0;
	virtual Result<void> Remove(const GameId& id) = 0;
};

}
