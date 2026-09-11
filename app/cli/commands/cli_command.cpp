#include "cli/commands/cli_command.h"

#include "CLI/App.hpp"

namespace emu {

bool CliCommand::IsSelected() const {
	return command_ != nullptr && command_->parsed();
}

}
