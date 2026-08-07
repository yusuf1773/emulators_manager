#pragma once

#include "icomand.hpp"
#include "../game_repo/game_repo.hpp"
#include "../game/game.hpp"
#include "../emulator/emulator.hpp"
#include "../launch_strategy/launch_strategy_manager.hpp"

class LaunchGameCommand : public CLIBAseCommand {
public:
    LaunchGameCommand(const GameRepository<Game>& games_repo,
        const GameRepository<Emulator>& emus_repo,
        const LaunchStrategyManager& launch_manager);
    void Register(CLI::App& app) override;
    void Execute() override;
private:
    const GameRepository<Game>& games_repo_;
    const GameRepository<Emulator>& emus_repo_;
    const LaunchStrategyManager& launch_manager_;
    std::string game_name_;
    std::string emu_name_;
};
