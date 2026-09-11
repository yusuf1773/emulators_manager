#include "cli/commands/add_game_command.h"

#include <utility>

#include "CLI/App.hpp"

namespace emu {

AddGameCommand::AddGameCommand(Repository<Game>& repo) : repo_(repo) {}

void AddGameCommand::Execute() {
  repo_.Add(Game{std::move(name_), std::move(path_), std::move(platform_)},
            allow_replace_);
}

void AddGameCommand::Register(CLI::App& app) {
  command_ = app.add_subcommand("add-game", "Add game in library");
  command_->add_option("--name", name_, "Game name")->required();
  command_->add_option("--platform", platform_, "Game platform");
  command_->add_option("--path", path_, "Game path")
      ->required()
      ->check(CLI::ExistingFile);
  command_->add_flag("--repl", allow_replace_,
                     "Allow replace element if already exist");
}

}
