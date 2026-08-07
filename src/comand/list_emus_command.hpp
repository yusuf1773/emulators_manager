#pragma once

#include "icomand.hpp"
#include "../emulator/emulator.hpp"
#include "../game_repo/game_repo.hpp"

class ListEmulatorsCommand : public CLIBAseCommand {
public:
    ListEmulatorsCommand(const GameRepository<Emulator>& emu_repo);
    void Execute() override;
    void Register(CLI::App& app) override;
private:
    const GameRepository<Emulator>& emu_repo_;
};