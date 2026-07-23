#pragma once

#include <CLI/App.hpp>

#include "icomand.hpp"
#include "../game/game.hpp"
#include "../game_repo/game_repo.hpp"

class ListGamesCommand : public ICommand {
public:
    ListGamesCommand(const GameRepository<Game>& game_repo);
    void Register(CLI::App &app) override;
    void Execute() override;
    bool IsSelected() const override;
private:
    const GameRepository<Game>& game_repo_;
    CLI::App* command_ = nullptr;
};