#include "add_emu_command.hpp"

AddEmulatorCommand::AddEmulatorCommand(GameRepository<Emulator>& emu_repo) :
    emu_repo_(emu_repo) {}

bool AddEmulatorCommand::IsSelected() const {
    return command_ != nullptr && command_->parsed();
}

void AddEmulatorCommand::Execute() {
    emu_repo_.Add(Emulator{name, path, platform, launch_strategy_id, args});
}

void AddEmulatorCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand("add-emu", "Add emulator");

    command_->add_option(
        "--name", name, "Emulator name"
    )->required();

    command_->add_option(
        "--path", path, "Emulator path"
    )->required()->check(CLI::ExistingFile);

    command_->add_option(
        "--platform", platform, "Emulator platform"
    );

    command_->add_option(
        "--launch-strategy-id", launch_strategy_id,
        "Emulator launch strategy id"
    );

    command_->add_option(
        "--arg", args, "Emulator extra args"
    );
}
