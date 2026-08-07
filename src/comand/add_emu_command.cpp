#include "add_emu_command.hpp"

#include <CLI/App.hpp>

AddEmulatorCommand::AddEmulatorCommand(GameRepository<Emulator>& emu_repo) :
    emu_repo_(emu_repo) {}

void AddEmulatorCommand::Execute() {
    emu_repo_.Add(Emulator{
        std::move(name), std::move(path), std::move(platform), std::move(launch_strategy_id), std::move(args)
    }, allow_replace_);
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

    command_->add_flag("--repl", allow_replace_, "Allow replace element if already exist");
}
