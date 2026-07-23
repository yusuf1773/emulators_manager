#pragma once

#include <CLI/App.hpp>

#include "icomand.hpp"
#include "../launch_strategy/launch_strategy_manager.hpp"

class ListLaunchStragiesCommand : public ICommand {
public:
    ListLaunchStragiesCommand(const LaunchStrategyManager& strategy_manager);
    void Execute() override;
    void Register(CLI::App &app) override;
    bool IsSelected() const override;
private:
    const LaunchStrategyManager& stragies_manager_;
    CLI::App* command_ = nullptr;
};
