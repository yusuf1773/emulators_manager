#pragma once

#include <algorithm>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "emu/application/repository.h"
#include "emu/persistence/json_storage.h"

namespace emu {

template <typename T>
class JsonRepository final : public Repository<T> {
	public:
	using NameGetter = std::function<const std::string&(const T&)>;
	JsonRepository(const std::filesystem::path& path, NameGetter getter)
		: storage_(path),
		  values_(storage_.Load()),
		  getter_(std::move(getter)) {}

	const std::vector<T>& GetAll() const override { return values_; }

	const T& GetByName(const std::string& name) const override {
		auto it = Find(values_, name);
		if (it == values_.end()) {
			throw std::runtime_error("Not found in repository " + name);
		}
		return *it;
	}

	void Add(const T& value, bool allow_replace = false) override {
		auto updated = values_;
		auto it = Find(updated, getter_(value));
		if (it != updated.end()) {
			if (!allow_replace)
				throw std::runtime_error("Duplicate value to add");
			*it = value;
		} else {
			updated.push_back(value);
		}
		Commit(updated);
	}

	void Replace(const std::string& old_name, const T& value) override {
		auto updated = values_;
		auto old = Find(updated, old_name);
		if (old == updated.end()) {
			throw std::runtime_error("Not found in repository " + old_name);
		}
		const auto duplicate = Find(updated, getter_(value));
		if (duplicate != updated.end() && duplicate != old) {
			throw std::runtime_error("Duplicate value to add");
		}

		updated.erase(old);
		updated.push_back(value);
		Commit(updated);
	}

	void RemoveByName(const std::string& name) override {
		auto updated = values_;
		auto it = Find(updated, name);
		if (it == updated.end()) {
			throw std::runtime_error("Not found in repository " + name);
		}
		updated.erase(it);
		Commit(updated);
	}

	private:
	template <typename Values>
	auto Find(Values& values, const std::string& name) const {
		return std::find_if(values.begin(), values.end(), [&](const T& value) {
			return getter_(value) == name;
		});
	}

	void Commit(std::vector<T>& updated) {
		storage_.Save(updated);
		values_.swap(updated);
	}

	JsonStorage<T> storage_;
	std::vector<T> values_;
	NameGetter getter_;
};

}
