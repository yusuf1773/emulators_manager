#pragma once

#include <CLI/App.hpp>
#include <concepts>
#include <memory>
#include <utility>
#include <vector>

#include "cli/commands/cli_command.h"

namespace emu {

class CommandManager {
public:
	template <typename Command, typename... Args>
		requires std::derived_from<Command, Command> &&
				 std::constructible_from<Command, Args&&...>
	void AddCommand(Args&&... args) {
		commands_.push_back(
			std::make_unique<Command>(std::forward<Args>(args)...));
	}

	void Register(CLI::App& cli) const;

	void RunSelected() const;

private:
	std::vector<std::unique_ptr<Command>> commands_;
};

}
