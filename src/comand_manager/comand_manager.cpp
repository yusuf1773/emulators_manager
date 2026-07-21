#include "comand_manager.hpp"

void CommandManager::Register(CLI::App& cli) const {
    for (auto& command : commands_) {
        command->Register(cli);
    }
}

void CommandManager::RunSelected() const {
    for (auto& command : commands_) {
        if (command->IsSelected()) {
            command->Execute();
            return;
        }
    }
}
