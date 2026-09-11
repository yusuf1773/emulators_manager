#pragma once

#include "cli/commands/cli_command.h"
#include "emu/application/emulator_service.h"

namespace emu {

class ListEmulatorsCommand : public CliCommand {
public:
	explicit ListEmulatorsCommand(const EmulatorService& service);
	void Execute() override;
	void Register(CLI::App& app) override;

private:
	const EmulatorService& service_;
};

}
