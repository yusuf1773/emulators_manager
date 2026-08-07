#include <iostream>

#include <CLI/App.hpp>

#include "list_games_command.hpp"

#include "../table_print/table.hpp"

ListGamesCommand::ListGamesCommand(const GameRepository<Game>& game_repo) :
    game_repo_(game_repo) {}

void ListGamesCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand("list-game",
        "Show list of games");
}

void ListGamesCommand::Execute() {
    const auto& games = game_repo_.GetAll();

    if (games.empty()) {
        std::cout << "Games not found" << std::endl;
        return;
    }

    Table<Game> games_table{
        {
            {"Name", [] (const Game& g) { return g.name; }},
            {"Platform", [] (const Game& g) { return g.platform; }},
            {"Path", [] (const Game& g) { return g.path.string(); } }
        }
    };

    games_table.print(games);
}
