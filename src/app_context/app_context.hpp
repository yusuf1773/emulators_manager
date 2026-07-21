#pragma once

#include "../app_config/config_data.hpp"
#include "../game_repo/game_repo.hpp"
#include "../game/game.hpp"
#include "../emulator/emulator.hpp"

class AppContext {
public:
    AppContext(ConfigData config);

    GameRepository<Game>& GetGameRepo();
    GameRepository<Emulator>& GetEmulatorsRepo();

    const ConfigData& GetConfig() const;
private:
    ConfigData config_data_;

    GameRepository<Game> game_repo_;
    GameRepository<Emulator> emu_repo_;
};