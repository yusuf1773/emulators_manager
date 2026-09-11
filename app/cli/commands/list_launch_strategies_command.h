#pragma once

#include "cli/commands/cli_command.h"
#include "emu/application/launch_strategy_registry.h"

namespace emu {

class ListLaunchStrategiesCommand : public CliCommand {
public:
	explicit ListLaunchStrategiesCommand(
		const LaunchStrategyRegistry& strategy_manager);
	void Execute() override;
	void Register(CLI::App& app) override;

private:
	const LaunchStrategyRegistry& strategies_manager_;
};

}
