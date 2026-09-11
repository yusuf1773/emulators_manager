#include "cli/commands/list_launch_strategies_command.h"

#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "CLI/App.hpp"
#include "cli/table.h"

namespace emu {

ListLaunchStrategiesCommand::ListLaunchStrategiesCommand(
	const LaunchStrategyRegistry& strategy_manager)
	: strategies_manager_(strategy_manager) {}

void ListLaunchStrategiesCommand::Register(CLI::App& app) {
	command_ =
		app.add_subcommand("list-strategies", "Show list of launch strategies");
}

void ListLaunchStrategiesCommand::Execute() {
	auto& strategies = strategies_manager_.GetAll();

	if (strategies.empty()) {
		std::cout << "Not found strategies\n";
	}

	auto infos_views =
		strategies | std::views::transform(
						 [](const std::unique_ptr<LaunchStrategy>& strategy) {
							 return strategy->Info();
						 });

	std::vector<LaunchStrategyInfo> infos(infos_views.begin(),
										  infos_views.end());
	Table<LaunchStrategyInfo> table_infos(
		{{"Id",
		  [](const LaunchStrategyInfo& info) { return std::string(info.id); }},
		 {"Name",
		  [](const LaunchStrategyInfo& info) {
			  return std::string(info.name);
		  }},
		 {"Description", [](const LaunchStrategyInfo& info) {
			  return std::string(info.description);
		  }}});

	table_infos.Print(infos);
}

}
