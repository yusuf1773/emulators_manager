#pragma once

#include "icomand.hpp"
#include "../game_repo/game_repo.hpp"
#include "../game/game.hpp"

class AddGameCommand : public ICommand {
public:
    AddGameCommand(GameRepository<Game>& repo);

    void Execute() override;

    void Register(CLI::App& app) override;

    bool IsSelected() const override;

private:
    GameRepository<Game>& repo_;

    CLI::App* command_ = nullptr;

    std::string name;
    std::filesystem::path path;
    std::string platform;
};
