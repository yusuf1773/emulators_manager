#pragma once

#include <filesystem>
#include <string>

#include "cli/commands/cli_command.h"
#include "emu/application/game_service.h"

namespace emu {

class ReplaceGameCommand : public CliCommand {
public:
	explicit ReplaceGameCommand(GameService& service);
	void Execute() override;
	void Register(CLI::App& app) override;

private:
	GameService& service_;
	std::string old_name_;
	std::string name_;
	std::filesystem::path path_;
	std::string platform_;
};

}
