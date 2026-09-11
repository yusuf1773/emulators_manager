#ifndef EMU_MANAGER_APP_CLI_COMMANDS_LIST_GAMES_COMMAND_H_
#define EMU_MANAGER_APP_CLI_COMMANDS_LIST_GAMES_COMMAND_H_

#include "cli/commands/cli_command.h"
#include "emu/application/repository.h"
#include "emu/domain/game.h"

namespace emu {

class ListGamesCommand : public CliCommand {
 public:
  explicit ListGamesCommand(const Repository<Game>& game_repo);
  void Register(CLI::App& app) override;
  void Execute() override;

 private:
  const Repository<Game>& game_repo_;
};

}

#endif
