#include "cli/commands/add_game_command.h"

#include <utility>

#include "CLI/App.hpp"
#include "cli/result.h"

namespace emu {

AddGameCommand::AddGameCommand(GameService& service) : service_(service) {}

void AddGameCommand::Execute() {
	if (allow_replace_) {
		auto existing = Require(service_.FindByName(name_));
		if (existing) {
			Require(service_.Update(existing->id, std::move(name_),
									std::move(path_), std::move(platform_)));
			return;
		}
	}
	Require(service_.Create(std::move(name_), std::move(path_),
							std::move(platform_)));
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
