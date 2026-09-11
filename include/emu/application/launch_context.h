#pragma once

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
