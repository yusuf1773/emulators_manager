#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "cli/commands/cli_command.h"
#include "emu/application/launch_profile_service.h"

namespace emu {

class AddLaunchProfileCommand : public CliCommand {
public:
	explicit AddLaunchProfileCommand(LaunchProfileService& service);
	void Execute() override;
	void Register(CLI::App& app) override;

private:
	LaunchProfileService& service_;
	std::string name_;
	std::string game_id_;
	std::string emulator_id_;
	std::string strategy_id_ = "direct";
	std::vector<std::string> arguments_;
	std::filesystem::path working_directory_;
	bool is_default_ = false;
};

}
