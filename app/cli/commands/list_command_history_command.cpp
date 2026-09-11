#include "cli/commands/list_command_history_command.h"

#include <chrono>
#include <format>
#include <iostream>

#include "CLI/App.hpp"
#include "cli/result.h"
#include "cli/table.h"

namespace emu {

ListCmdHistoryCommand::ListCmdHistoryCommand(HistoryRepository& history_repo,
											 std::size_t limit)
	: history_repo_(history_repo), limit_(limit) {}

void ListCmdHistoryCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("list-history", "History list");
}

void ListCmdHistoryCommand::Execute() {
	const auto histories = Require(history_repo_.FindRecent(limit_));

	if (histories.empty()) {
		std::cout << "History is empty" << std::endl;
		return;
	}

	Table<CommandHistoryEntry> table_history{
		{{"Command",
		  [](const CommandHistoryEntry& history) { return history.command; }},
		 {"Timestamp", [](const CommandHistoryEntry& history) {
			  auto local_zone = std::chrono::current_zone();
			  auto local_time = local_zone->to_local(history.timestamp);
			  auto rounded_time =
				  std::chrono::time_point_cast<std::chrono::seconds>(
					  local_time);
			  return std::format("{:%F %T}", rounded_time);
		  }}}};

	table_history.Print(histories);
}

}
