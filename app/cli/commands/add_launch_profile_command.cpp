#include "cli/commands/add_launch_profile_command.h"

#include <utility>

#include "CLI/App.hpp"
#include "cli/result.h"

namespace emu {

AddLaunchProfileCommand::AddLaunchProfileCommand(LaunchProfileService& service)
	: service_(service) {}

void AddLaunchProfileCommand::Execute() {
	Require(service_.Create(std::move(name_), GameId(std::move(game_id_)),
							EmulatorId(std::move(emulator_id_)),
							std::move(strategy_id_), std::move(arguments_),
							std::move(working_directory_), is_default_));
}

void AddLaunchProfileCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("add-profile", "Add launch profile");
	command_->add_option("--name", name_, "Profile name")->required();
	command_->add_option("--game-id", game_id_, "Game id")->required();
	command_->add_option("--emulator-id", emulator_id_, "Emulator id")
		->required();
	command_->add_option("--strategy-id", strategy_id_, "Launch strategy id");
	command_->add_option("--arg", arguments_, "Profile argument");
	command_->add_option("--working-directory", working_directory_,
						 "Process working directory");
	command_->add_flag("--default", is_default_, "Default profile for game");
}

}
