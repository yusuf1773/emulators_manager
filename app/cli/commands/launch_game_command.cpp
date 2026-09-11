#include "cli/commands/launch_game_command.h"

#include "CLI/App.hpp"
#include "cli/result.h"

namespace emu {

LaunchGameCommand::LaunchGameCommand(LaunchGameService& service)
	: service_(service) {}

void LaunchGameCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("launch", "Launch game");
	command_->add_option("--profile-id", profile_id_, "Launch profile id")
		->required();
}

void LaunchGameCommand::Execute() {
	const auto result = Require(service_.Launch(LaunchProfileId(profile_id_)));
	if (result.exit_code != 0) {
		throw std::runtime_error("Process exited with code " +
								 std::to_string(result.exit_code));
	}
}

}
