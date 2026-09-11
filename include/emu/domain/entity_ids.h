#pragma once

#include <compare>
#include <string>
#include <utility>

namespace emu {

template <typename Tag>
class EntityId {
public:
	EntityId() = default;
	explicit EntityId(std::string value) : value_(std::move(value)) {}

	const std::string& value() const { return value_; }
	bool empty() const { return value_.empty(); }

	auto operator<=>(const EntityId&) const = default;

private:
	std::string value_;
};

struct GameIdTag;
struct EmulatorIdTag;
struct LaunchProfileIdTag;

using GameId = EntityId<GameIdTag>;
using EmulatorId = EntityId<EmulatorIdTag>;
using LaunchProfileId = EntityId<LaunchProfileIdTag>;

}
