#ifndef EMU_MANAGER_APP_CLI_COMMANDS_CLI_COMMAND_H_
#define EMU_MANAGER_APP_CLI_COMMANDS_CLI_COMMAND_H_
#include <CLI/App.hpp>

namespace emu {

class Command {
 public:
  virtual ~Command() = default;
  virtual void Register(CLI::App& app) = 0;
  virtual void Execute() = 0;
  virtual bool IsSelected() const = 0;
};

class CliCommand : public Command {
 public:
  bool IsSelected() const override;

 protected:
  CLI::App* command_ = nullptr;
};

}

#endif
