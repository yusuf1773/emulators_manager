#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "CLI/CLI.hpp"
#include "bootstrap/app_context.h"
#include "cli/command_manager.h"
#include "emu/application/launch_game_service.h"
#include "emu/application/launch_strategy_registry.h"
#include "emu/launch/system_process_runner.h"
#include "emu/persistence/config.h"

namespace emu {

class Application {
	public:
	Application(std::string name, const std::filesystem::path& config_path);

	int Run(int argc, char** argv);

	private:
	AppContext context_;
	CLI::App cli_;
	CommandManager command_manager_;
	LaunchStrategyRegistry launch_strategies_;
	SystemProcessRunner process_runner_;
	LaunchGameService launch_service_;

	private:
	void AddCommands();
	void AddLaunchStrategies();
};

}
