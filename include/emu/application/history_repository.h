#ifndef EMU_MANAGER_INCLUDE_EMU_APPLICATION_HISTORY_REPOSITORY_H_
#define EMU_MANAGER_INCLUDE_EMU_APPLICATION_HISTORY_REPOSITORY_H_

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

#endif
