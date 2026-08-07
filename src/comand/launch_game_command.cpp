#include "launch_game_command.hpp"

#include <CLI/App.hpp>

#include "../launch_strategy/istrategy.hpp"

LaunchGameCommand::LaunchGameCommand(const GameRepository<Game>& games_repo,
    const GameRepository<Emulator>& emus_repo,
    const LaunchStrategyManager& launch_manager) :
    games_repo_(games_repo), emus_repo_(emus_repo), launch_manager_(launch_manager) {}
    
void LaunchGameCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand("launch", "Launch game");

    command_->add_option(
        "--game", game_name_, "Game name that launching"
    )->required();

    command_->add_option(
        "--emu", emu_name_, "Emulator name that launching game"
    )->required();
}

void LaunchGameCommand::Execute() {
    auto game = games_repo_.GetById(game_name_);
    auto emu = emus_repo_.GetById(emu_name_);
    auto& launch_strategy = launch_manager_.GetById(emu.launch_strategy_id);
    LaunchContext context{
        emu, game
    };
    launch_strategy.Launch(context);
}
