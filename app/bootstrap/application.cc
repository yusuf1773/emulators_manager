#include "bootstrap/application.h"

#include <filesystem>
#include <string>
#include <utility>

#include "cli/commands/add_emu_command.h"
#include "cli/commands/add_game_command.h"
#include "cli/commands/launch_game_command.h"
#include "cli/commands/list_command_history_command.h"
#include "cli/commands/list_emus_command.h"
#include "cli/commands/list_games_command.h"
#include "cli/commands/list_launch_strategies_command.h"
#include "cli/commands/replace_emu_command.h"
#include "cli/commands/replace_game_command.h"
#include "emu/launch/direct_strategy.h"

namespace emu {

Application::Application(std::string name,
                         const std::filesystem::path& config_path)
    : context_{Config{config_path}.GetData()},
      cli_{std::move(name)},
      launch_service_(context_.GetGameRepo(), context_.GetEmulatorsRepo(),
                      launch_strategies_, process_runner_) {
  AddCommands();
  AddLaunchStrategies();

  command_manager_.Register(cli_);
}

void Application::AddCommands() {
  command_manager_.AddCommand<AddGameCommand>(context_.GetGameRepo());
  command_manager_.AddCommand<ListGamesCommand>(context_.GetGameRepo());
  command_manager_.AddCommand<ListLaunchStrategiesCommand>(launch_strategies_);
  command_manager_.AddCommand<AddEmulatorCommand>(context_.GetEmulatorsRepo());
  command_manager_.AddCommand<ListEmulatorsCommand>(
      context_.GetEmulatorsRepo());
  command_manager_.AddCommand<LaunchGameCommand>(launch_service_);
  command_manager_.AddCommand<ReplaceEmuCommand>(context_.GetEmulatorsRepo());
  command_manager_.AddCommand<ReplaceGameCommand>(context_.GetGameRepo());
  command_manager_.AddCommand<ListCmdHistoryCommand>(
      context_.GetCommandsHistoryRepo());
}

void Application::AddLaunchStrategies() {
  launch_strategies_.Add<DirectLaunchStrategy>();
}

int Application::Run(int argc, char** argv) {
  try {
    cli_.parse(argc, argv);
    std::string command;
    for (int i = 1; i < argc; ++i) {
      if (i > 1) command += ' ';
      command += argv[i];
    }
    context_.GetCommandsHistoryRepo().Add(
        CommandHistoryEntry{std::move(command)});
    command_manager_.RunSelected();
    return 0;
  } catch (const CLI::ParseError& e) {
    return cli_.exit(e);
  }
}

}
