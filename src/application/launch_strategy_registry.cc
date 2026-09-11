#include "emu/application/launch_strategy_registry.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace emu {

void LaunchStrategyRegistry::AddStrategy(
    std::unique_ptr<LaunchStrategy> strategy) {
  const auto id = strategy->Info().id;

  const auto it = std::find_if(
      strategies_.begin(), strategies_.end(),
      [id](const auto& current) { return current->Info().id == id; });

  if (it != strategies_.end()) {
    throw std::invalid_argument("Launch strategy already exists: " +
                                std::string(id));
  }

  strategies_.push_back(std::move(strategy));
}

const std::vector<std::unique_ptr<LaunchStrategy>>&
LaunchStrategyRegistry::GetAll() const {
  return strategies_;
}

const LaunchStrategy& LaunchStrategyRegistry::GetById(
    std::string_view id) const {
  auto it = std::find_if(strategies_.begin(), strategies_.end(),
                         [&](const std::unique_ptr<LaunchStrategy>& strategy) {
                           return strategy->Info().id == id;
                         });
  if (it == strategies_.end()) {
    throw std::invalid_argument("Not found strategy");
  }
  return **it;
}

}
