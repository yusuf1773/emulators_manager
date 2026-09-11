#include "cli/commands/add_emu_command.h"

#include <utility>

#include "CLI/App.hpp"

namespace emu {

AddEmulatorCommand::AddEmulatorCommand(Repository<Emulator>& emu_repo)
	: emu_repo_(emu_repo) {}

void AddEmulatorCommand::Execute() {
	emu_repo_.Add(
		Emulator{std::move(name_), std::move(path_), std::move(platform_),
				 std::move(launch_strategy_id_), std::move(args_)},
		allow_replace_);
}

void AddEmulatorCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("add-emu", "Add emulator");

	command_->add_option("--name", name_, "Emulator name")->required();

	command_->add_option("--path", path_, "Emulator path")
		->required()
		->check(CLI::ExistingFile);

	command_->add_option("--platform", platform_, "Emulator platform");

	command_->add_option("--launch-strategy-id", launch_strategy_id_,
						 "Emulator launch strategy id");

	command_->add_option("--arg", args_, "Emulator extra args");

	command_->add_flag("--repl", allow_replace_,
					   "Allow replace element if already exist");
}

}
