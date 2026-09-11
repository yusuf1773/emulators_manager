#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "emu/application/application_error.h"
#include "emu/domain/launch_profile.h"

namespace emu {

class LaunchProfileRepository {
public:
	virtual ~LaunchProfileRepository() = default;
	virtual Result<std::vector<LaunchProfile>> FindAll() const = 0;
	virtual Result<std::vector<LaunchProfile>> FindByGame(
		const GameId& game_id) const = 0;
	virtual Result<std::optional<LaunchProfile>> FindById(
		const LaunchProfileId& id) const = 0;
	virtual Result<std::optional<LaunchProfile>> FindByName(
		const GameId& game_id, std::string_view name) const = 0;
	virtual Result<void> Save(const LaunchProfile& profile) = 0;
	virtual Result<void> Remove(const LaunchProfileId& id) = 0;
};

}
