#include "comand_manager.hpp"

#include "../comand/add_game_command.hpp"

CommandManager::CommandManager(AppContext& context) {
    AddComand<AddGameCommand>(context.GetGameRepo());
}

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
