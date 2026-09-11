#pragma once

#include "cli/commands/cli_command.h"
#include "emu/application/repository.h"
#include "emu/domain/emulator.h"

namespace emu {

class ListEmulatorsCommand : public CliCommand {
	public:
	explicit ListEmulatorsCommand(const Repository<Emulator>& emu_repo);
	void Execute() override;
	void Register(CLI::App& app) override;

	private:
	const Repository<Emulator>& emu_repo_;
};

}
