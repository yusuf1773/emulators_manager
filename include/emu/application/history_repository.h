#pragma once

#include <deque>

#include "emu/domain/command_history_entry.h"

namespace emu {

class HistoryRepository {
	public:
	virtual ~HistoryRepository() = default;
	virtual void Add(CommandHistoryEntry entry) = 0;
	virtual const std::deque<CommandHistoryEntry>& GetHistories() const = 0;
};

}
