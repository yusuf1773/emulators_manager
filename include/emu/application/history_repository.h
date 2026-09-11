#pragma once

#include <cstddef>
#include <vector>

#include "emu/application/application_error.h"
#include "emu/domain/command_history_entry.h"

namespace emu {

class HistoryRepository {
public:
	virtual ~HistoryRepository() = default;
	virtual Result<void> Add(const CommandHistoryEntry& entry) = 0;
	virtual Result<std::vector<CommandHistoryEntry>> FindRecent(
		std::size_t limit) const = 0;
	virtual Result<void> Trim(std::size_t limit) = 0;
};

}
