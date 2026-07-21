#pragma once

#include <memory>
#include <vector>

#include <CLI/CLI.hpp>

#include "../comand/icomand.hpp"
#include "../app_context/app_context.hpp"

class CommandManager {
public:
    CommandManager(AppContext& context);

    template<typename Command, typename... Args>
    void AddComand(Args&&... args) {
        commands_.push_back(
            std::make_unique<Command>(
                std::forward<Args>(args)...
            )
        );
    }

    void Register(CLI::App& cli) const;

    void RunSelected() const;

private:
    std::vector<std::unique_ptr<ICommand>> commands_;
};