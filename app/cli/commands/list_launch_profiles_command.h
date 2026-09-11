#pragma once

#include <string>

#include "cli/commands/cli_command.h"
#include "emu/application/launch_profile_service.h"

namespace emu {

class ListLaunchProfilesCommand : public CliCommand {
public:
	explicit ListLaunchProfilesCommand(const LaunchProfileService& service);
	void Execute() override;
	void Register(CLI::App& app) override;

private:
	const LaunchProfileService& service_;
	std::string game_id_;
};

}
