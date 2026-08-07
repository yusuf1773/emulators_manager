#include "add_game_command.hpp"

#include <CLI/App.hpp>

AddGameCommand::AddGameCommand(GameRepository<Game>& repo) : repo_(repo) {}

void AddGameCommand::Execute() {
    repo_.Add(Game{
        std::move(name), std::move(path), std::move(platform)
    }, allow_replace_);
}

void AddGameCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand("add-game", "Add game in library");
    command_->add_option("--name", name, "Game name")->required();
    command_->add_option("--platform", platform, "Game platform");
    command_->add_option("--path", path, "Game path")->required()->check(CLI::ExistingFile);
    command_->add_flag("--repl", allow_replace_, "Allow replace element if already exist");
}