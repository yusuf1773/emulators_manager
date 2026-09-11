#include "cli/commands/list_launch_profiles_command.h"

#include <iostream>
#include <string>

#include "CLI/App.hpp"
#include "cli/result.h"
#include "cli/table.h"

namespace emu {

ListLaunchProfilesCommand::ListLaunchProfilesCommand(
	const LaunchProfileService& service)
	: service_(service) {}

void ListLaunchProfilesCommand::Register(CLI::App& app) {
	command_ = app.add_subcommand("list-profiles", "Launch profiles list");
	command_->add_option("--game-id", game_id_, "Filter by game id");
}

void ListLaunchProfilesCommand::Execute() {
	const auto profiles = game_id_.empty()
							  ? Require(service_.List())
							  : Require(service_.ListByGame(GameId(game_id_)));
	if (profiles.empty()) {
		std::cout << "Launch profiles not found" << std::endl;
		return;
	}
	Table<LaunchProfile> table{
		{"Id", [](const LaunchProfile& profile) { return profile.id.value(); }},
		{"Name", [](const LaunchProfile& profile) { return profile.name; }},
		{"Game Id",
		 [](const LaunchProfile& profile) { return profile.game_id.value(); }},
		{"Emulator Id",
		 [](const LaunchProfile& profile) {
			 return profile.emulator_id.value();
		 }},
		{"Strategy",
		 [](const LaunchProfile& profile) { return profile.strategy_id; }},
		{"Default", [](const LaunchProfile& profile) {
			 return profile.is_default ? "yes" : "no";
		 }}};
	table.Print(profiles);
}

}
