#include "cli/commands/replace_game_command.h"

#include <utility>

#include "CLI/App.hpp"

namespace emu {

ReplaceGameCommand::ReplaceGameCommand(Repository<Game>& repo) : repo_(repo) {}

void ReplaceGameCommand::Execute() {
	repo_.Replace(old_name_, Game{std::move(name_), std::move(path_),
								  std::move(platform_)});
}

void ReplaceGameCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("replace-game", "Replace game by name");
	command_->add_option("--old_name", old_name_, "Game old name")->required();
	command_->add_option("--name", name_, "Game new name")->required();
	command_->add_option("--platform", platform_, "Game new platform");
	command_->add_option("--path", path_, "Game new path")
		->required()
		->check(CLI::ExistingFile);
}

}
