#pragma once

#include <algorithm>
#include <concepts>
#include <memory>
#include <vector>
#include <string_view>

#include "istrategy.hpp"

class LaunchStrategyManager {
public:
    template<typename Strategy, typename... Args>
    requires std::derived_from<Strategy, ILaunchStrategy> &&
        std::constructible_from<Strategy, Args&&...>
    void Add(Args&&... args) {
        auto strategy = std::make_unique<Strategy>(
            std::forward<Args>(args)...
        );
        AddStrategy(std::move(strategy));
    }
    const ILaunchStrategy& GetById(std::string_view id) const;
    const std::vector<std::unique_ptr<ILaunchStrategy>>& GetAll() const;
private:
    void AddStrategy(std::unique_ptr<ILaunchStrategy> strategy);
    std::vector<std::unique_ptr<ILaunchStrategy>> strategies_;
};