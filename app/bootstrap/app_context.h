#ifndef EMU_MANAGER_APP_BOOTSTRAP_APP_CONTEXT_H_
#define EMU_MANAGER_APP_BOOTSTRAP_APP_CONTEXT_H_

#include "emu/domain/command_history_entry.h"
#include "emu/domain/emulator.h"
#include "emu/domain/game.h"
#include "emu/persistence/config_data.h"
#include "emu/persistence/json_history_repository.h"
#include "emu/persistence/json_repository.h"

namespace emu {

class AppContext {
 public:
  explicit AppContext(ConfigData config);

  JsonRepository<Game>& GetGameRepo();
  JsonRepository<Emulator>& GetEmulatorsRepo();
  JsonHistoryRepository& GetCommandsHistoryRepo();

  const ConfigData& GetConfig() const;

 private:
  ConfigData config_data_;

  JsonRepository<Game> game_repo_;
  JsonRepository<Emulator> emu_repo_;
  JsonHistoryRepository history_repo_;
};

}

#endif
