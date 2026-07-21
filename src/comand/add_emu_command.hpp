#pragma once

#include "CLI/App.hpp"

#include "icomand.hpp"
#include "../game_repo/game_repo.hpp"
#include "../emulator/emulator.hpp"

class AddEmulatorCommand : public ICommand {
public:
    AddEmulatorCommand(GameRepository<Emulator>& emu_repo);
    void Execute() override;
    void Register(CLI::App &app) override;
    bool IsSelected() const override;
private:
    GameRepository<Emulator>& emu_repo_;
    CLI::App* command_ = nullptr;
    std::string name;
    std::filesystem::path path;
    std::string platform;
    std::string launch_strategy_id;
    std::vector<std::string> args;
};
