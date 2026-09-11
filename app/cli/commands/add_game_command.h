#pragma once

#include <filesystem>
#include <string>

#include "cli/commands/cli_command.h"
#include "emu/application/repository.h"
#include "emu/domain/game.h"

namespace emu {

class AddGameCommand : public CliCommand {
	public:
	explicit AddGameCommand(Repository<Game>& repo);

	void Execute() override;

	void Register(CLI::App& app) override;

	private:
	Repository<Game>& repo_;
	std::string name_;
	std::filesystem::path path_;
	std::string platform_;
	bool allow_replace_ = false;
};

}
