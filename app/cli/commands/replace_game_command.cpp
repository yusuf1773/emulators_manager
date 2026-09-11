#include "cli/commands/replace_game_command.h"

#include <stdexcept>
#include <utility>

#include "CLI/App.hpp"
#include "cli/result.h"

namespace emu {

ReplaceGameCommand::ReplaceGameCommand(GameService& service)
	: service_(service) {}

void ReplaceGameCommand::Execute() {
	auto existing = Require(service_.FindByName(old_name_));
	if (!existing) throw std::runtime_error("Game not found");
	Require(service_.Update(existing->id, std::move(name_), std::move(path_),
							std::move(platform_)));
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
