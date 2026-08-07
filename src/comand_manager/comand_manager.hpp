#pragma once

#include <concepts>
#include <memory>
#include <vector>

#include "../comand/icomand.hpp"

namespace CLI {
    class App;
}

class CommandManager {
public:
    template<typename Command, typename... Args>
    requires std::derived_from<Command, ICommand> &&
        std::constructible_from<Command, Args&&...>
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