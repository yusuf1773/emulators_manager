#include "replace_emu_command.hpp"

#include <CLI/App.hpp>

ReplaceEmuCommand::ReplaceEmuCommand(GameRepository<Emulator>& emu_repo) :
    emu_repo_(emu_repo) {}

void ReplaceEmuCommand::Execute() {
    emu_repo_.RemoveByName(old_name);
    emu_repo_.Add(Emulator{
        std::move(name), std::move(path), std::move(platform), std::move(launch_strategy_id), std::move(args)
    });
}

void ReplaceEmuCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand("replace-emu", "Replace emulator by name");

    command_->add_option(
        "--old_name", old_name, "Emulator old name"
    )->required();

    command_->add_option(
        "--name", name, "Emulator new name"
    )->required();

    command_->add_option(
        "--path", path, "Emulator new path"
    )->required()->check(CLI::ExistingFile);

    command_->add_option(
        "--platform", platform, "Emulator new platform"
    );

    command_->add_option(
        "--launch-strategy-id", launch_strategy_id,
        "Emulator new launch strategy id"
    );

    command_->add_option(
        "--arg", args, "Emulator new extra args"
    );
}
