#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "../emulator/emulator.hpp"
#include "../game/game.hpp"

struct LaunchContext {
    const Emulator& emu;
    const Game& game;
};
