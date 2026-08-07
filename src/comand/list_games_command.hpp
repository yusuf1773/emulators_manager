#pragma once

#include "icomand.hpp"
#include "../game/game.hpp"
#include "../game_repo/game_repo.hpp"

class ListGamesCommand : public CLIBAseCommand {
public:
    ListGamesCommand(const GameRepository<Game>& game_repo);
    void Register(CLI::App& app) override;
    void Execute() override;
private:
    const GameRepository<Game>& game_repo_;
};