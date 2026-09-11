#include "cli/commands/list_games_command.h"

#include <iostream>

#include "CLI/App.hpp"
#include "cli/result.h"
#include "cli/table.h"

namespace emu {

ListGamesCommand::ListGamesCommand(const GameService& service)
	: service_(service) {}

void ListGamesCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("list-game", "Show list of games");
}

void ListGamesCommand::Execute() {
	const auto games = Require(service_.List());
	if (games.empty()) {
		std::cout << "Games not found" << std::endl;
		return;
	}
	Table<Game> table{
		{{"Id", [](const Game& game) { return game.id.value(); }},
		 {"Name", [](const Game& game) { return game.name; }},
		 {"Platform", [](const Game& game) { return game.platform; }},
		 {"Path",
		  [](const Game& game) { return game.content_path.string(); }}}};
	table.Print(games);
}

}
