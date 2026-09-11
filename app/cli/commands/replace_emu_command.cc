#include "cli/commands/replace_emu_command.h"

#include <utility>

#include "CLI/App.hpp"

namespace emu {

ReplaceEmuCommand::ReplaceEmuCommand(Repository<Emulator>& emu_repo)
    : emu_repo_(emu_repo) {}

void ReplaceEmuCommand::Execute() {
  emu_repo_.Replace(
      old_name_,
      Emulator{std::move(name_), std::move(path_), std::move(platform_),
               std::move(launch_strategy_id_), std::move(args_)});
}

void ReplaceEmuCommand::Register(CLI::App& app) {
  command_ = app.add_subcommand("replace-emu", "Replace emulator by name");

  command_->add_option("--old_name", old_name_, "Emulator old name")
      ->required();

  command_->add_option("--name", name_, "Emulator new name")->required();

  command_->add_option("--path", path_, "Emulator new path")
      ->required()
      ->check(CLI::ExistingFile);

  command_->add_option("--platform", platform_, "Emulator new platform");

  command_->add_option("--launch-strategy-id", launch_strategy_id_,
                       "Emulator new launch strategy id");

  command_->add_option("--arg", args_, "Emulator new extra args");
}

}
