#include "replace_game_command.hpp"

#include <CLI/App.hpp>

ReplaceGameCommand::ReplaceGameCommand(GameRepository<Game>& repo) : repo_(repo) {}

void ReplaceGameCommand::Execute() {
    repo_.RemoveByName(old_name);
    repo_.Add(Game{
        std::move(name), std::move(path), std::move(platform)
    });
}

void ReplaceGameCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand("replace-game", "Replace game by name");
    command_->add_option("--old_name", old_name, "Game old name")->required();
    command_->add_option("--name", name, "Game new name")->required();
    command_->add_option("--platform", platform, "Game new platform");
    command_->add_option("--path", path, "Game new path")->required()->check(CLI::ExistingFile);
}