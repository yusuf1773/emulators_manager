#include "launch_strategy_manager.hpp"

#include <algorithm>

void LaunchStrategyManager::AddStrategy(std::unique_ptr<ILaunchStrategy> strategy) {
    const auto id = strategy->Info().id;

    const auto it = std::find_if(
        strategies_.begin(),
        strategies_.end(),
        [id](const auto& current) {
            return current->Info().id == id;
        }
    );

    if (it != strategies_.end()) {
        throw std::invalid_argument(
            "Launch strategy already exists: " +
            std::string(id)
        );
    }

    strategies_.push_back(std::move(strategy));
}

const std::vector<std::unique_ptr<ILaunchStrategy>>& LaunchStrategyManager::GetAll() const {
    return strategies_;
}

const ILaunchStrategy& LaunchStrategyManager::GetById(std::string_view id) const {
    auto it = std::find_if(
        strategies_.begin(), strategies_.end(),
        [&] (const std::unique_ptr<ILaunchStrategy>& strategy) {
            return strategy->Info().id == id;
        }
    );
    if (it == strategies_.end()) {
        throw std::invalid_argument("Not found strategy");
    }
    return **it;
}
