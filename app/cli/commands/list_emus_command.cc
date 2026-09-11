#include "cli/commands/list_emus_command.h"

#include <iostream>
#include <ranges>
#include <string>

#include "CLI/App.hpp"
#include "cli/table.h"

namespace emu {

ListEmulatorsCommand::ListEmulatorsCommand(const Repository<Emulator>& emu_repo)
    : emu_repo_(emu_repo) {}

void ListEmulatorsCommand::Register(CLI::App& app) {
  command_ = app.add_subcommand("list-emu", "Emulators list");
}

void ListEmulatorsCommand::Execute() {
  auto emus = emu_repo_.GetAll();

  if (emus.empty()) {
    std::cout << "Emulators not found" << std::endl;
    return;
  }

  Table<Emulator> table_emus{
      {"Name", [](const Emulator& e) { return e.name; }},
      {"Platform", [](const Emulator& e) { return e.platform; }},
      {"Path", [](const Emulator& e) { return e.path.string(); }},
      {"Strategy ID", [](const Emulator& e) { return e.launch_strategy_id; }},
      {"Arguments", [](const Emulator& e) {
         return e.args.empty() ? ""
                               : e.args | std::views::join_with(' ') |
                                     std::ranges::to<std::string>();
       }}};

  table_emus.Print(emus);
}

}
