#include "icomand.hpp"

#include <CLI/App.hpp>

bool CLIBAseCommand::IsSelected() const {
    return command_ != nullptr && command_->parsed();
}