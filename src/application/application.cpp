#include "application.hpp"

#include "../comand/add_game_command.hpp"
#include "../comand/list_games_command.hpp"
#include "../comand/add_emu_command.hpp"
#include "../comand/list_launch_stategies_command.h"
#include "../comand/list_emus_command.hpp"
#include "../comand/launch_game_command.hpp"

#include "../launch_strategy/direct_strategy.hpp"


Application::Application(std::string name,
                         const std::filesystem::path& config_path) :
    cli_{std::move(name)}, context_{Config{config_path}.GetData()} {

    AddComands();
    AddLaunchStrategies();

    command_manager_.Register(cli_);
}

void Application::AddComands() {
    command_manager_.AddComand<AddGameCommand>(context_.GetGameRepo());
    command_manager_.AddComand<ListGamesCommand>(context_.GetGameRepo());
    command_manager_.AddComand<ListLaunchStragiesCommand>(launch_strategies_);
    command_manager_.AddComand<AddEmulatorCommand>(context_.GetEmulatorsRepo());
    command_manager_.AddComand<ListEmulatorsCommand>(context_.GetEmulatorsRepo());
    command_manager_.AddComand<LaunchGameCommand>(
        context_.GetGameRepo(),
        context_.GetEmulatorsRepo(),
        launch_strategies_
    );
}

void Application::AddLaunchStrategies() {
    launch_strategies_.Add<DirectLaunchStrategy>();
}

void Application::Run(int argc, char** argv) {
    try {
        cli_.parse(argc, argv);
        command_manager_.RunSelected();
    } catch (const CLI::ParseError& e) {
        cli_.exit(e);
    }
}