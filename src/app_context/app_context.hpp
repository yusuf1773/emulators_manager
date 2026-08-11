#pragma once

#include "../game_repo/str_line_history.hpp"
#include "../app_config/config_data.hpp"
#include "../game_repo/game_repo.hpp"
#include "../game/game.hpp"
#include "../emulator/emulator.hpp"
#include "../game_repo/history_repo.hpp"

class AppContext {
public:
    AppContext(ConfigData config);

    GameRepository<Game>& GetGameRepo();
    GameRepository<Emulator>& GetEmulatorsRepo();
    HistoryRepository<StrLineHistory>& GetCommandsHistoryRepo();

    const ConfigData& GetConfig() const;
private:
    ConfigData config_data_;

    GameRepository<Game> game_repo_;
    GameRepository<Emulator> emu_repo_;
    HistoryRepository<StrLineHistory> history_repo_;
};