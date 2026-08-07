#pragma once

#include "icomand.hpp"
#include "../launch_strategy/launch_strategy_manager.hpp"

class ListLaunchStragiesCommand : public CLIBAseCommand {
public:
    ListLaunchStragiesCommand(const LaunchStrategyManager& strategy_manager);
    void Execute() override;
    void Register(CLI::App& app) override;
private:
    const LaunchStrategyManager& stragies_manager_;
};
