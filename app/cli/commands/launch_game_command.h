#pragma once

#include <string>

#include "cli/commands/cli_command.h"
#include "emu/application/launch_game_service.h"

namespace emu {

class LaunchGameCommand : public CliCommand {
	public:
	explicit LaunchGameCommand(LaunchGameService& service);
	void Execute() override;
	void Register(CLI::App& app) override;

	private:
	LaunchGameService& service_;
	std::string game_name_;
	std::string emu_name_;
};

}
