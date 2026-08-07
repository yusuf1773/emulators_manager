#pragma once

#include "icomand.hpp"
#include "../game_repo/game_repo.hpp"
#include "../emulator/emulator.hpp"

class AddEmulatorCommand : public CLIBAseCommand {
public:
    AddEmulatorCommand(GameRepository<Emulator>& emu_repo);
    void Execute() override;
    void Register(CLI::App& app) override;
private:
    GameRepository<Emulator>& emu_repo_;
    std::string name;
    std::filesystem::path path;
    std::string platform;
    std::string launch_strategy_id;
    std::vector<std::string> args;
    bool allow_replace_ = false;
};
