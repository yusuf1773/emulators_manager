#include "app_context.hpp"

AppContext::AppContext(ConfigData config) :
    config_data_(std::move(config)),
    game_repo_(config_data_.games_path),
    emu_repo_(config_data_.emulators_path) {}

GameRepository<Game>& AppContext::GetGameRepo() {
    return game_repo_;
}

GameRepository<Emulator>& AppContext::GetEmulatorsRepo() {
    return emu_repo_;
}

const ConfigData& AppContext::GetConfig() const {
    return config_data_;
}
