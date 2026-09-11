#ifndef EMU_MANAGER_APP_CLI_COMMANDS_ADD_EMU_COMMAND_H_
#define EMU_MANAGER_APP_CLI_COMMANDS_ADD_EMU_COMMAND_H_

#include <filesystem>
#include <string>
#include <vector>

#include "cli/commands/cli_command.h"
#include "emu/application/repository.h"
#include "emu/domain/emulator.h"

namespace emu {

class AddEmulatorCommand : public CliCommand {
 public:
  explicit AddEmulatorCommand(Repository<Emulator>& emu_repo);
  void Execute() override;
  void Register(CLI::App& app) override;

 private:
  Repository<Emulator>& emu_repo_;
  std::string name_;
  std::filesystem::path path_;
  std::string platform_;
  std::string launch_strategy_id_;
  std::vector<std::string> args_;
  bool allow_replace_ = false;
};

}

#endif
