#include "list_launch_stategies_command.h"

#include <ranges>
#include "../table_print/table.hpp"

ListLaunchStragiesCommand::ListLaunchStragiesCommand(const LaunchStrategyManager& strategy_manager) :
    stragies_manager_(strategy_manager) {}

bool ListLaunchStragiesCommand::IsSelected() const {
    return command_ != nullptr && command_->parsed();
}

void ListLaunchStragiesCommand::Register(CLI::App& app) {
    command_ = app.add_subcommand(
        "list-strategies",
        "Show list of launch strategies"
    );
}

void ListLaunchStragiesCommand::Execute() {
    auto& strategies = stragies_manager_.GetAll();

    if (strategies.empty()) {
        std::cout << "Not found strategies\n";
    }

    auto infos_views = strategies | std::views::transform(
        [] (const std::unique_ptr<ILaunchStrategy>& strategy) {
            return strategy->Info();
        }
    );

    std::vector<LaunchStrategyInfo> infos(infos_views.begin(), infos_views.end());
    Table<LaunchStrategyInfo> table_infos(
        {
            {"Id", [] (const LaunchStrategyInfo& info) { return std::string(info.id); } },
            {"Name", [] (const LaunchStrategyInfo& info) { return std::string(info.name); } },
            {"Description", [] (const LaunchStrategyInfo& info) { return std::string(info.description); } }
        }  
    );

    table_infos.print(infos);
    
}
