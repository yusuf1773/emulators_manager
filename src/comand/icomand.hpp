#pragma once

namespace CLI {
    class App;
}

class ICommand {
public:
    virtual ~ICommand() {}
    virtual void Register(CLI::App& app) = 0;
    virtual void Execute() = 0;
    virtual bool IsSelected() const = 0;
};

class CLIBAseCommand : public ICommand {
    bool IsSelected() const;
protected:
    CLI::App* command_ = nullptr;
};