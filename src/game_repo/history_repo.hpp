#pragma once

#include <filesystem>
#include <deque>

#include "constrains.hpp"
#include "json_storage.hpp"

template<typename History, typename Storage = JsonStorage<History, std::deque<History>>>
requires IsStorage<Storage, History, std::deque<History>>
class HistoryRepository {
public:
    HistoryRepository(const std::filesystem::path& path, size_t max_size) : 
        storage_{path}, histories_(storage_.Load()), max_size_(max_size) {
    }
    
    void Add(History hist) {
        if (histories_.size() >= max_size_) {
            histories_.pop_front();
        }
        histories_.push_back(std::move(hist));
        storage_.Save(histories_);
    }

    const std::deque<History>& GetHistories() const {
        return histories_;
    }

    const History& GetHistoryByInd(size_t ind) const {
        return histories_.at(ind);
    }

private:
    Storage storage_;
    std::deque<History> histories_;
    size_t max_size_;
};
