#pragma once

#include <CLI/CLI.hpp>

class ICommand {
public:
    virtual ~ICommand() {}
    virtual void Register(CLI::App& app) = 0;
    virtual void Execute() = 0;
    virtual bool IsSelected() const = 0;
};
