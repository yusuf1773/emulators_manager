#pragma once

#include <cstddef>

#include "cli/commands/cli_command.h"
#include "emu/application/history_repository.h"
#include "emu/domain/command_history_entry.h"

namespace emu {

class ListCmdHistoryCommand : public CliCommand {
public:
	ListCmdHistoryCommand(HistoryRepository& history_repo, std::size_t limit);
	void Register(CLI::App& app) override;
	void Execute() override;

private:
	HistoryRepository& history_repo_;
	std::size_t limit_;
};

}
