#ifndef EMU_MANAGER_APP_CLI_COMMANDS_LIST_LAUNCH_STRATEGIES_COMMAND_H_
#define EMU_MANAGER_APP_CLI_COMMANDS_LIST_LAUNCH_STRATEGIES_COMMAND_H_

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

#endif
