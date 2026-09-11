#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "cli/commands/cli_command.h"
#include "emu/application/emulator_service.h"

namespace emu {

class ReplaceEmuCommand : public CliCommand {
public:
	explicit ReplaceEmuCommand(EmulatorService& service);
	void Execute() override;
	void Register(CLI::App& app) override;

private:
	EmulatorService& service_;
	std::string old_name_;
	std::string name_;
	std::filesystem::path path_;
	std::string platform_;
	std::vector<std::string> arguments_;
};

}
