#ifndef EMU_MANAGER_INCLUDE_EMU_APPLICATION_LAUNCH_CONTEXT_H_
#define EMU_MANAGER_INCLUDE_EMU_APPLICATION_LAUNCH_CONTEXT_H_

#include <filesystem>
#include <string>
#include <vector>

#include "emu/domain/emulator.h"
#include "emu/domain/game.h"

namespace emu {

struct LaunchContext {
  const Emulator& emu;
  const Game& game;
};

}

#endif
