#include "cli/commands/launch_game_command.h"

#include "CLI/App.hpp"

namespace emu {

LaunchGameCommand::LaunchGameCommand(LaunchGameService& service)
	: service_(service) {}

void LaunchGameCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("launch", "Launch game");
	command_->add_option("--game", game_name_, "Game name that launching")
		->required();
	command_
		->add_option("--emu", emu_name_, "Emulator name that launching game")
		->required();
}

void LaunchGameCommand::Execute() { service_.Launch(game_name_, emu_name_); }

}
