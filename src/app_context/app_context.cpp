#include "app_context.hpp"

AppContext::AppContext(ConfigData config) :
    config_data_(std::move(config)),
    game_repo_(config_data_.games_path,
        [] (const Game& game) -> const std::string& { return game.name; }
    ),
    emu_repo_(config_data_.emulators_path,
        [] (const Emulator& emu) -> const std::string& { return emu.name; }
    ),
    history_repo_(config_data_.history_path, config_data_.history_max_size)
    {}

GameRepository<Game>& AppContext::GetGameRepo() {
    return game_repo_;
}

GameRepository<Emulator>& AppContext::GetEmulatorsRepo() {
    return emu_repo_;
}

HistoryRepository<StrLineHistory>& AppContext::GetCommandsHistoryRepo() {
    return history_repo_;
}

const ConfigData& AppContext::GetConfig() const {
    return config_data_;
}
