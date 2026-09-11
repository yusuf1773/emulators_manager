#ifndef EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_JSON_HISTORY_REPOSITORY_H_
#define EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_JSON_HISTORY_REPOSITORY_H_

#include <cstddef>
#include <deque>
#include <filesystem>
#include <utility>

#include "emu/application/history_repository.h"
#include "emu/persistence/json_storage.h"

namespace emu {

class JsonHistoryRepository final : public HistoryRepository {
 public:
  JsonHistoryRepository(const std::filesystem::path& path, std::size_t max_size)
      : storage_(path), histories_(storage_.Load()), max_size_(max_size) {
    if (histories_.size() > max_size_) {
      histories_.resize(max_size_);
      storage_.Save(histories_);
    }
  }

  void Add(CommandHistoryEntry entry) override {
    if (max_size_ == 0) return;
    auto updated = histories_;
    updated.push_front(std::move(entry));
    if (updated.size() > max_size_) updated.resize(max_size_);
    storage_.Save(updated);
    histories_.swap(updated);
  }

  const std::deque<CommandHistoryEntry>& GetHistories() const override {
    return histories_;
  }

 private:
  JsonStorage<CommandHistoryEntry, std::deque<CommandHistoryEntry>> storage_;
  std::deque<CommandHistoryEntry> histories_;
  std::size_t max_size_;
};

}

#endif
