#pragma once

#include <algorithm>
#include <concepts>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "emu/application/launch_strategy.h"

namespace emu {

class LaunchStrategyRegistry {
public:
	template <typename Strategy, typename... Args>
		requires std::derived_from<Strategy, LaunchStrategy> &&
				 std::constructible_from<Strategy, Args&&...>
	void Add(Args&&... args) {
		auto strategy = std::make_unique<Strategy>(std::forward<Args>(args)...);
		AddStrategy(std::move(strategy));
	}
	const LaunchStrategy& GetById(std::string_view id) const;
	const std::vector<std::unique_ptr<LaunchStrategy>>& GetAll() const;

private:
	void AddStrategy(std::unique_ptr<LaunchStrategy> strategy);
	std::vector<std::unique_ptr<LaunchStrategy>> strategies_;
};

}
