#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "emu/application/emulator_repository.h"
#include "emu/application/game_repository.h"
#include "emu/application/id_generator.h"
#include "emu/application/launch_profile_repository.h"
#include "emu/application/process_runner.h"

namespace emu::testing {

class FixedIdGenerator final : public IdGenerator {
public:
	explicit FixedIdGenerator(std::vector<std::string> ids)
		: ids_(std::move(ids)) {}

	std::string Next() override {
		std::string value = ids_.at(index_);
		++index_;
		return value;
	}

private:
	std::vector<std::string> ids_;
	std::size_t index_ = 0;
};

class MemoryGameRepository final : public GameRepository {
public:
	Result<std::vector<Game>> FindAll() const override {
		if (find_all_error) {
			return Failure(ErrorCode::kPersistence, "find all failed");
		}
		return values;
	}
	Result<std::optional<Game>> FindById(const GameId& id) const override {
		return Find([&](const Game& game) { return game.id == id; });
	}
	Result<std::optional<Game>> FindByName(
		std::string_view name) const override {
		return Find([&](const Game& game) { return game.name == name; });
	}
	Result<void> Save(const Game& game) override {
		if (save_error) return Failure(ErrorCode::kPersistence, "save failed");
		auto found = std::find_if(
			values.begin(), values.end(),
			[&](const Game& value) { return value.id == game.id; });
		if (found == values.end())
			values.push_back(game);
		else
			*found = game;
		return {};
	}
	Result<void> Remove(const GameId& id) override {
		auto found =
			std::find_if(values.begin(), values.end(),
						 [&](const Game& value) { return value.id == id; });
		if (found == values.end())
			return Failure(ErrorCode::kNotFound, "missing");
		values.erase(found);
		return {};
	}

	std::vector<Game> values;
	bool find_all_error = false;
	bool save_error = false;

private:
	template <typename Predicate>
	Result<std::optional<Game>> Find(Predicate predicate) const {
		auto found = std::find_if(values.begin(), values.end(), predicate);
		if (found == values.end()) return std::optional<Game>{};
		return std::optional<Game>(*found);
	}
};

class MemoryEmulatorRepository final : public EmulatorRepository {
public:
	Result<std::vector<Emulator>> FindAll() const override { return values; }
	Result<std::optional<Emulator>> FindById(
		const EmulatorId& id) const override {
		return Find(
			[&](const Emulator& emulator) { return emulator.id == id; });
	}
	Result<std::optional<Emulator>> FindByName(
		std::string_view name) const override {
		return Find(
			[&](const Emulator& emulator) { return emulator.name == name; });
	}
	Result<void> Save(const Emulator& emulator) override {
		auto found = std::find_if(
			values.begin(), values.end(),
			[&](const Emulator& value) { return value.id == emulator.id; });
		if (found == values.end())
			values.push_back(emulator);
		else
			*found = emulator;
		return {};
	}
	Result<void> Remove(const EmulatorId& id) override {
		auto found =
			std::find_if(values.begin(), values.end(),
						 [&](const Emulator& value) { return value.id == id; });
		if (found == values.end())
			return Failure(ErrorCode::kNotFound, "missing");
		values.erase(found);
		return {};
	}

	std::vector<Emulator> values;

private:
	template <typename Predicate>
	Result<std::optional<Emulator>> Find(Predicate predicate) const {
		auto found = std::find_if(values.begin(), values.end(), predicate);
		if (found == values.end()) return std::optional<Emulator>{};
		return std::optional<Emulator>(*found);
	}
};

class MemoryLaunchProfileRepository final : public LaunchProfileRepository {
public:
	Result<std::vector<LaunchProfile>> FindAll() const override {
		return values;
	}
	Result<std::vector<LaunchProfile>> FindByGame(
		const GameId& game_id) const override {
		std::vector<LaunchProfile> found;
		for (const auto& profile : values) {
			if (profile.game_id == game_id) found.push_back(profile);
		}
		return found;
	}
	Result<std::optional<LaunchProfile>> FindById(
		const LaunchProfileId& id) const override {
		return Find(
			[&](const LaunchProfile& profile) { return profile.id == id; });
	}
	Result<std::optional<LaunchProfile>> FindByName(
		const GameId& game_id, std::string_view name) const override {
		return Find([&](const LaunchProfile& profile) {
			return profile.game_id == game_id && profile.name == name;
		});
	}
	Result<void> Save(const LaunchProfile& profile) override {
		if (profile.is_default) {
			for (auto& value : values) {
				if (value.game_id == profile.game_id) value.is_default = false;
			}
		}
		auto found = std::find_if(
			values.begin(), values.end(),
			[&](const LaunchProfile& value) { return value.id == profile.id; });
		if (found == values.end())
			values.push_back(profile);
		else
			*found = profile;
		return {};
	}
	Result<void> Remove(const LaunchProfileId& id) override {
		auto found = std::find_if(
			values.begin(), values.end(),
			[&](const LaunchProfile& value) { return value.id == id; });
		if (found == values.end())
			return Failure(ErrorCode::kNotFound, "missing");
		values.erase(found);
		return {};
	}

	std::vector<LaunchProfile> values;

private:
	template <typename Predicate>
	Result<std::optional<LaunchProfile>> Find(Predicate predicate) const {
		auto found = std::find_if(values.begin(), values.end(), predicate);
		if (found == values.end()) return std::optional<LaunchProfile>{};
		return std::optional<LaunchProfile>(*found);
	}
};

class RecordingProcessRunner final : public ProcessRunner {
public:
	Result<ProcessResult> Run(const LaunchRequest& request) override {
		requests.push_back(request);
		if (error) return Failure(ErrorCode::kProcess, "process failed");
		return ProcessResult{exit_code};
	}

	std::vector<LaunchRequest> requests;
	bool error = false;
	int exit_code = 0;
};

}
