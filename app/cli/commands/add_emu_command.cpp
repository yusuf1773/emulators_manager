#include "cli/commands/add_emu_command.h"

#include <utility>

#include "CLI/App.hpp"
#include "cli/result.h"

namespace emu {

AddEmulatorCommand::AddEmulatorCommand(EmulatorService& service)
	: service_(service) {}

void AddEmulatorCommand::Execute() {
	if (allow_replace_) {
		auto existing = Require(service_.FindByName(name_));
		if (existing) {
			Require(service_.Update(existing->id, std::move(name_),
									std::move(path_), std::move(platform_),
									std::move(arguments_)));
			return;
		}
	}
	Require(service_.Create(std::move(name_), std::move(path_),
							std::move(platform_), std::move(arguments_)));
}

void AddEmulatorCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("add-emu", "Add emulator");
	command_->add_option("--name", name_, "Emulator name")->required();
	command_->add_option("--path", path_, "Emulator path")
		->required()
		->check(CLI::ExistingFile);
	command_->add_option("--platform", platform_, "Emulator platform");
	command_->add_option("--arg", arguments_, "Emulator base argument");
	command_->add_flag("--repl", allow_replace_,
					   "Allow replace element if already exist");
}

}
