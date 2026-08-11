#pragma once

#include "icomand.hpp"
#include "../game_repo/history_repo.hpp"
#include "../game_repo/str_line_history.hpp"

class ListCmdHistoryCommand : public CLIBAseCommand {
public:
    ListCmdHistoryCommand(HistoryRepository<StrLineHistory>& history_repo);
    void Register(CLI::App& app) override;
    void Execute() override;
private:
    HistoryRepository<StrLineHistory>& history_repo_;
};
