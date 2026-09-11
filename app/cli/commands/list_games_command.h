#pragma once

#include "cli/commands/cli_command.h"
#include "emu/application/game_service.h"

namespace emu {

class ListGamesCommand : public CliCommand {
public:
	explicit ListGamesCommand(const GameService& service);
	void Register(CLI::App& app) override;
	void Execute() override;

private:
	const GameService& service_;
};

}
