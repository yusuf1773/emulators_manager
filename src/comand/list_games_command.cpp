#include <iostream>

#include "list_games_command.hpp"

ListGamesCommand::ListGamesCommand(GameRepository<Game>& game_repo) :
    game_repo_(game_repo) {}

bool ListGamesCommand::IsSelected() const {
    return command_ != nullptr && command_->parsed();
}

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

    const std::string kName = "Name";
    const std::string kPlatform = "Platform";
    const std::string kPath = "Path";

    size_t name_size = kName.size();
    size_t platform_size = kPlatform.size();
    size_t path_size = kPath.size();

    for (auto& game : games) {
        name_size = std::max(name_size, game.name.size());
        platform_size = std::max(platform_size, game.platform.size());
        path_size = std::max(path_size, game.path.string().size());
    }

    const size_t kBoardPadding = 2;

    name_size += kBoardPadding;
    platform_size += kBoardPadding;
    path_size += kBoardPadding;

    const size_t kTableWidth = name_size + platform_size + path_size;

    std::cout << std::left;
    std::cout << std::setw(name_size) << kName;
    std::cout << std::setw(platform_size) << kPlatform;
    std::cout << std::setw(path_size) << kPath << '\n';
    std::cout << std::string(kTableWidth, '-') << '\n';

    for (auto& game : games) {
        std::cout << std::setw(name_size) << game.name;
        std::cout << std::setw(platform_size) << game.platform;
        std::cout << std::setw(path_size) << game.path.string();
        std::cout << std::endl;
    }
}
