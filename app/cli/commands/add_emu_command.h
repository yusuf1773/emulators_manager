#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "cli/commands/cli_command.h"
#include "emu/application/emulator_service.h"

namespace emu {

class AddEmulatorCommand : public CliCommand {
public:
	explicit AddEmulatorCommand(EmulatorService& service);
	void Execute() override;
	void Register(CLI::App& app) override;

private:
	EmulatorService& service_;
	std::string name_;
	std::filesystem::path path_;
	std::string platform_;
	std::vector<std::string> arguments_;
	bool allow_replace_ = false;
};

}
