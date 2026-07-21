#pragma once

#include <filesystem>
#include <memory>

#include <CLI/CLI.hpp>

#include "../app_config/config.hpp"
#include "../app_context/app_context.hpp"
#include "../comand_manager/comand_manager.hpp"
#include "../launch_strategy/launch_strategy_manager.hpp"

class Application {
public:
    Application(std::string name,
        const std::filesystem::path& config_path);

    void Run(int argc, char** argv);

private:
    AppContext context_;
    CLI::App cli_;
    CommandManager command_manager_;
    LaunchStrategyManager launch_strategies_;
private:
    void AddComands();
    void AddLaunchStrategies();
};
