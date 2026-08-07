#pragma once

#include "icomand.hpp"
#include "../game_repo/game_repo.hpp"
#include "../game/game.hpp"

class AddGameCommand : public CLIBAseCommand {
public:
    AddGameCommand(GameRepository<Game>& repo);

    void Execute() override;

    void Register(CLI::App& app) override;
private:
    GameRepository<Game>& repo_;
    std::string name;
    std::filesystem::path path;
    std::string platform;
    bool allow_replace_ = false;
};
