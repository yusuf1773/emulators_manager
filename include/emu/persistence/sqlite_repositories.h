#pragma once

#include "emu/application/emulator_repository.h"
#include "emu/application/game_repository.h"
#include "emu/application/history_repository.h"
#include "emu/application/launch_profile_repository.h"
#include "emu/persistence/sqlite_database.h"

namespace emu {

class SqliteGameRepository final : public GameRepository {
public:
	explicit SqliteGameRepository(SqliteDatabase& database);
	Result<std::vector<Game>> FindAll() const override;
	Result<std::optional<Game>> FindById(const GameId& id) const override;
	Result<std::optional<Game>> FindByName(
		std::string_view name) const override;
	Result<void> Save(const Game& game) override;
	Result<void> Remove(const GameId& id) override;

private:
	SqliteDatabase& database_;
};

class SqliteEmulatorRepository final : public EmulatorRepository {
public:
	explicit SqliteEmulatorRepository(SqliteDatabase& database);
	Result<std::vector<Emulator>> FindAll() const override;
	Result<std::optional<Emulator>> FindById(
		const EmulatorId& id) const override;
	Result<std::optional<Emulator>> FindByName(
		std::string_view name) const override;
	Result<void> Save(const Emulator& emulator) override;
	Result<void> Remove(const EmulatorId& id) override;

private:
	SqliteDatabase& database_;
};

class SqliteLaunchProfileRepository final : public LaunchProfileRepository {
public:
	explicit SqliteLaunchProfileRepository(SqliteDatabase& database);
	Result<std::vector<LaunchProfile>> FindAll() const override;
	Result<std::vector<LaunchProfile>> FindByGame(
		const GameId& game_id) const override;
	Result<std::optional<LaunchProfile>> FindById(
		const LaunchProfileId& id) const override;
	Result<std::optional<LaunchProfile>> FindByName(
		const GameId& game_id, std::string_view name) const override;
	Result<void> Save(const LaunchProfile& profile) override;
	Result<void> Remove(const LaunchProfileId& id) override;

private:
	SqliteDatabase& database_;
};

class SqliteHistoryRepository final : public HistoryRepository {
public:
	explicit SqliteHistoryRepository(SqliteDatabase& database);
	Result<void> Add(const CommandHistoryEntry& entry) override;
	Result<std::vector<CommandHistoryEntry>> FindRecent(
		std::size_t limit) const override;
	Result<void> Trim(std::size_t limit) override;

private:
	SqliteDatabase& database_;
};

}
