#include "cli/commands/replace_emu_command.h"

#include <stdexcept>
#include <utility>

#include "CLI/App.hpp"
#include "cli/result.h"

namespace emu {

ReplaceEmuCommand::ReplaceEmuCommand(EmulatorService& service)
	: service_(service) {}

void ReplaceEmuCommand::Execute() {
	auto existing = Require(service_.FindByName(old_name_));
	if (!existing) throw std::runtime_error("Emulator not found");
	Require(service_.Update(existing->id, std::move(name_), std::move(path_),
							std::move(platform_), std::move(arguments_)));
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
	command_->add_option("--arg", arguments_, "Emulator base argument");
}

}
