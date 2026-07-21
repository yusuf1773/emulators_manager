#include "add_game_command.hpp"

AddGameCommand::AddGameCommand(GameRepository<Game>& repo) : repo_(repo) {}

void AddGameCommand::Execute() {
    repo_.Add(Game{name, path, platform});
}

void AddGameCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand("add-game", "Add game in library");
    command_->add_option("--name", name, "Game name")->required();
    command_->add_option("--platform", platform, "Game platform");
    command_->add_option("--path", path, "Game path")->required()->check(CLI::ExistingFile);
}

bool AddGameCommand::IsSelected() const {
    return command_ != nullptr && command_->parsed();
}
