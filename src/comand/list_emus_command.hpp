#pragma once

#include <CLI/App.hpp>

#include "icomand.hpp"
#include "../emulator/emulator.hpp"
#include "../game_repo/game_repo.hpp"

class ListEmulatorsCommand : public ICommand {
public:
    ListEmulatorsCommand(GameRepository<Emulator>& emu_repo);
    void Execute() override;
    void Register(CLI::App& app) override;
    bool IsSelected() const override;
private:
    GameRepository<Emulator>& emu_repo_;
    CLI::App* command_ = nullptr;
};