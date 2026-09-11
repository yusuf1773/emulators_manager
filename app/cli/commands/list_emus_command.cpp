#include "cli/commands/list_emus_command.h"

#include <iostream>
#include <ranges>
#include <string>

#include "CLI/App.hpp"
#include "cli/result.h"
#include "cli/table.h"

namespace emu {

ListEmulatorsCommand::ListEmulatorsCommand(const EmulatorService& service)
	: service_(service) {}

void ListEmulatorsCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("list-emu", "Emulators list");
}

void ListEmulatorsCommand::Execute() {
	const auto emulators = Require(service_.List());
	if (emulators.empty()) {
		std::cout << "Emulators not found" << std::endl;
		return;
	}
	Table<Emulator> table{
		{"Id", [](const Emulator& emulator) { return emulator.id.value(); }},
		{"Name", [](const Emulator& emulator) { return emulator.name; }},
		{"Platform",
		 [](const Emulator& emulator) { return emulator.platform; }},
		{"Path",
		 [](const Emulator& emulator) {
			 return emulator.executable_path.string();
		 }},
		{"Arguments", [](const Emulator& emulator) {
			 return emulator.base_arguments.empty()
						? ""
						: emulator.base_arguments | std::views::join_with(' ') |
							  std::ranges::to<std::string>();
		 }}};
	table.Print(emulators);
}

}
