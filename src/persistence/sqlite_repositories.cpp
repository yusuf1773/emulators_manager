#include "emu/persistence/sqlite_repositories.h"

#include <sqlite3.h>

#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "nlohmann/json.hpp"

namespace emu {
namespace {

class Statement {
public:
	Statement(sqlite3* database, const char* sql) : database_(database) {
		if (sqlite3_prepare_v2(database, sql, -1, &statement_, nullptr) !=
			SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(database));
		}
	}

	~Statement() { sqlite3_finalize(statement_); }

	void Bind(int index, std::string_view value) {
		if (sqlite3_bind_text(statement_, index, value.data(),
							  static_cast<int>(value.size()),
							  SQLITE_TRANSIENT) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(database_));
		}
	}

	void Bind(int index, int value) {
		if (sqlite3_bind_int(statement_, index, value) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(database_));
		}
	}

	void Bind(int index, sqlite3_int64 value) {
		if (sqlite3_bind_int64(statement_, index, value) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(database_));
		}
	}

	bool Step() {
		const int result = sqlite3_step(statement_);
		if (result == SQLITE_ROW) return true;
		if (result == SQLITE_DONE) return false;
		throw std::runtime_error(sqlite3_errmsg(database_));
	}

	std::string Text(int column) const {
		const auto* value = sqlite3_column_text(statement_, column);
		return value == nullptr ? "" : reinterpret_cast<const char*>(value);
	}

	int Integer(int column) const {
		return sqlite3_column_int(statement_, column);
	}
	sqlite3_int64 Integer64(int column) const {
		return sqlite3_column_int64(statement_, column);
	}

private:
	sqlite3* database_;
	sqlite3_stmt* statement_ = nullptr;
};

std::unexpected<ApplicationError> PersistenceFailure(
	const std::exception& error) {
	return Failure(ErrorCode::kPersistence, error.what());
}

Game ReadGame(const Statement& statement) {
	return {GameId(statement.Text(0)), statement.Text(1), statement.Text(2),
			statement.Text(3)};
}

Emulator ReadEmulator(const Statement& statement) {
	return {EmulatorId(statement.Text(0)), statement.Text(1), statement.Text(2),
			statement.Text(3),
			nlohmann::json::parse(statement.Text(4))
				.get<std::vector<std::string>>()};
}

LaunchProfile ReadProfile(const Statement& statement) {
	return {LaunchProfileId(statement.Text(0)),
			statement.Text(1),
			GameId(statement.Text(2)),
			EmulatorId(statement.Text(3)),
			statement.Text(4),
			nlohmann::json::parse(statement.Text(5))
				.get<std::vector<std::string>>(),
			statement.Text(6),
			statement.Integer(7) != 0};
}

template <typename Entity, typename Reader>
Result<std::vector<Entity>> FindMany(sqlite3* database, const char* sql,
									 Reader reader) {
	try {
		Statement statement(database, sql);
		std::vector<Entity> values;
		while (statement.Step()) values.push_back(reader(statement));
		return values;
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

void Execute(sqlite3* database, const char* sql) {
	char* error = nullptr;
	if (sqlite3_exec(database, sql, nullptr, nullptr, &error) != SQLITE_OK) {
		const std::string message =
			error == nullptr ? sqlite3_errmsg(database) : error;
		sqlite3_free(error);
		throw std::runtime_error(message);
	}
}

}

SqliteGameRepository::SqliteGameRepository(SqliteDatabase& database)
	: database_(database) {}

Result<std::vector<Game>> SqliteGameRepository::FindAll() const {
	return FindMany<Game>(
		database_.handle(),
		"SELECT id,name,content_path,platform FROM games ORDER BY name",
		ReadGame);
}

Result<std::optional<Game>> SqliteGameRepository::FindById(
	const GameId& id) const {
	try {
		Statement statement(
			database_.handle(),
			"SELECT id,name,content_path,platform FROM games WHERE id=?");
		statement.Bind(1, id.value());
		if (!statement.Step()) return std::optional<Game>{};
		return std::optional<Game>(ReadGame(statement));
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<std::optional<Game>> SqliteGameRepository::FindByName(
	std::string_view name) const {
	try {
		Statement statement(
			database_.handle(),
			"SELECT id,name,content_path,platform FROM games WHERE name=? "
			"COLLATE NOCASE");
		statement.Bind(1, name);
		if (!statement.Step()) return std::optional<Game>{};
		return std::optional<Game>(ReadGame(statement));
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<void> SqliteGameRepository::Save(const Game& game) {
	try {
		Statement statement(
			database_.handle(),
			"INSERT INTO games(id,name,content_path,platform) VALUES(?,?,?,?) "
			"ON CONFLICT(id) DO UPDATE SET name=excluded.name,"
			"content_path=excluded.content_path,platform=excluded.platform");
		statement.Bind(1, game.id.value());
		statement.Bind(2, game.name);
		statement.Bind(3, game.content_path.string());
		statement.Bind(4, game.platform);
		statement.Step();
		return {};
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<void> SqliteGameRepository::Remove(const GameId& id) {
	try {
		Statement statement(database_.handle(), "DELETE FROM games WHERE id=?");
		statement.Bind(1, id.value());
		statement.Step();
		if (sqlite3_changes(database_.handle()) == 0) {
			return Failure(ErrorCode::kNotFound, "Game not found");
		}
		return {};
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

SqliteEmulatorRepository::SqliteEmulatorRepository(SqliteDatabase& database)
	: database_(database) {}

Result<std::vector<Emulator>> SqliteEmulatorRepository::FindAll() const {
	return FindMany<Emulator>(
		database_.handle(),
		"SELECT id,name,executable_path,platform,base_arguments FROM emulators "
		"ORDER BY name",
		ReadEmulator);
}

Result<std::optional<Emulator>> SqliteEmulatorRepository::FindById(
	const EmulatorId& id) const {
	try {
		Statement statement(
			database_.handle(),
			"SELECT id,name,executable_path,platform,base_arguments FROM "
			"emulators WHERE id=?");
		statement.Bind(1, id.value());
		if (!statement.Step()) return std::optional<Emulator>{};
		return std::optional<Emulator>(ReadEmulator(statement));
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<std::optional<Emulator>> SqliteEmulatorRepository::FindByName(
	std::string_view name) const {
	try {
		Statement statement(
			database_.handle(),
			"SELECT id,name,executable_path,platform,base_arguments FROM "
			"emulators WHERE name=? COLLATE NOCASE");
		statement.Bind(1, name);
		if (!statement.Step()) return std::optional<Emulator>{};
		return std::optional<Emulator>(ReadEmulator(statement));
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<void> SqliteEmulatorRepository::Save(const Emulator& emulator) {
	try {
		Statement statement(
			database_.handle(),
			"INSERT INTO "
			"emulators(id,name,executable_path,platform,base_arguments) "
			"VALUES(?,?,?,?,?) ON CONFLICT(id) DO UPDATE SET "
			"name=excluded.name,"
			"executable_path=excluded.executable_path,platform=excluded."
			"platform,"
			"base_arguments=excluded.base_arguments");
		statement.Bind(1, emulator.id.value());
		statement.Bind(2, emulator.name);
		statement.Bind(3, emulator.executable_path.string());
		statement.Bind(4, emulator.platform);
		statement.Bind(5, nlohmann::json(emulator.base_arguments).dump());
		statement.Step();
		return {};
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<void> SqliteEmulatorRepository::Remove(const EmulatorId& id) {
	try {
		Statement statement(database_.handle(),
							"DELETE FROM emulators WHERE id=?");
		statement.Bind(1, id.value());
		statement.Step();
		if (sqlite3_changes(database_.handle()) == 0) {
			return Failure(ErrorCode::kNotFound, "Emulator not found");
		}
		return {};
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

SqliteLaunchProfileRepository::SqliteLaunchProfileRepository(
	SqliteDatabase& database)
	: database_(database) {}

Result<std::vector<LaunchProfile>> SqliteLaunchProfileRepository::FindAll()
	const {
	return FindMany<LaunchProfile>(
		database_.handle(),
		"SELECT id,name,game_id,emulator_id,strategy_id,arguments,"
		"working_directory,is_default FROM launch_profiles ORDER BY name",
		ReadProfile);
}

Result<std::vector<LaunchProfile>> SqliteLaunchProfileRepository::FindByGame(
	const GameId& game_id) const {
	try {
		Statement statement(
			database_.handle(),
			"SELECT id,name,game_id,emulator_id,strategy_id,arguments,"
			"working_directory,is_default FROM launch_profiles WHERE game_id=? "
			"ORDER BY is_default DESC,name");
		statement.Bind(1, game_id.value());
		std::vector<LaunchProfile> values;
		while (statement.Step()) values.push_back(ReadProfile(statement));
		return values;
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<std::optional<LaunchProfile>> SqliteLaunchProfileRepository::FindById(
	const LaunchProfileId& id) const {
	try {
		Statement statement(
			database_.handle(),
			"SELECT id,name,game_id,emulator_id,strategy_id,arguments,"
			"working_directory,is_default FROM launch_profiles WHERE id=?");
		statement.Bind(1, id.value());
		if (!statement.Step()) return std::optional<LaunchProfile>{};
		return std::optional<LaunchProfile>(ReadProfile(statement));
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<std::optional<LaunchProfile>> SqliteLaunchProfileRepository::FindByName(
	const GameId& game_id, std::string_view name) const {
	try {
		Statement statement(
			database_.handle(),
			"SELECT id,name,game_id,emulator_id,strategy_id,arguments,"
			"working_directory,is_default FROM launch_profiles WHERE game_id=? "
			"AND name=? COLLATE NOCASE");
		statement.Bind(1, game_id.value());
		statement.Bind(2, name);
		if (!statement.Step()) return std::optional<LaunchProfile>{};
		return std::optional<LaunchProfile>(ReadProfile(statement));
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<void> SqliteLaunchProfileRepository::Save(const LaunchProfile& profile) {
	try {
		Execute(database_.handle(), "BEGIN IMMEDIATE");
		if (profile.is_default) {
			Statement clear(
				database_.handle(),
				"UPDATE launch_profiles SET is_default=0 WHERE game_id=?");
			clear.Bind(1, profile.game_id.value());
			clear.Step();
		}
		Statement statement(
			database_.handle(),
			"INSERT INTO "
			"launch_profiles(id,name,game_id,emulator_id,strategy_id,"
			"arguments,working_directory,is_default) VALUES(?,?,?,?,?,?,?,?) "
			"ON CONFLICT(id) DO UPDATE SET name=excluded.name,"
			"game_id=excluded.game_id,emulator_id=excluded.emulator_id,"
			"strategy_id=excluded.strategy_id,arguments=excluded.arguments,"
			"working_directory=excluded.working_directory,"
			"is_default=excluded.is_default");
		statement.Bind(1, profile.id.value());
		statement.Bind(2, profile.name);
		statement.Bind(3, profile.game_id.value());
		statement.Bind(4, profile.emulator_id.value());
		statement.Bind(5, profile.strategy_id);
		statement.Bind(6, nlohmann::json(profile.arguments).dump());
		statement.Bind(7, profile.working_directory.string());
		statement.Bind(8, profile.is_default ? 1 : 0);
		statement.Step();
		Execute(database_.handle(), "COMMIT");
		return {};
	} catch (const std::exception& error) {
		try {
			Execute(database_.handle(), "ROLLBACK");
		} catch (...) {
		}
		return PersistenceFailure(error);
	}
}

Result<void> SqliteLaunchProfileRepository::Remove(const LaunchProfileId& id) {
	try {
		Statement statement(database_.handle(),
							"DELETE FROM launch_profiles WHERE id=?");
		statement.Bind(1, id.value());
		statement.Step();
		if (sqlite3_changes(database_.handle()) == 0) {
			return Failure(ErrorCode::kNotFound, "Launch profile not found");
		}
		return {};
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

SqliteHistoryRepository::SqliteHistoryRepository(SqliteDatabase& database)
	: database_(database) {}

Result<void> SqliteHistoryRepository::Add(const CommandHistoryEntry& entry) {
	try {
		Statement statement(
			database_.handle(),
			"INSERT INTO command_history(command,timestamp) VALUES(?,?)");
		statement.Bind(1, entry.command);
		statement.Bind(2, static_cast<sqlite3_int64>(
							  std::chrono::duration_cast<std::chrono::seconds>(
								  entry.timestamp.time_since_epoch())
								  .count()));
		statement.Step();
		return {};
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<std::vector<CommandHistoryEntry>> SqliteHistoryRepository::FindRecent(
	std::size_t limit) const {
	try {
		Statement statement(database_.handle(),
							"SELECT command,timestamp FROM command_history "
							"ORDER BY id DESC LIMIT ?");
		statement.Bind(1, static_cast<int>(limit));
		std::vector<CommandHistoryEntry> values;
		while (statement.Step()) {
			values.emplace_back(
				statement.Text(0),
				std::chrono::system_clock::time_point(
					std::chrono::seconds(statement.Integer64(1))));
		}
		return values;
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

Result<void> SqliteHistoryRepository::Trim(std::size_t limit) {
	try {
		Statement statement(
			database_.handle(),
			"DELETE FROM command_history WHERE id NOT IN (SELECT id FROM "
			"command_history ORDER BY id DESC LIMIT ?)");
		statement.Bind(1, static_cast<int>(limit));
		statement.Step();
		return {};
	} catch (const std::exception& error) {
		return PersistenceFailure(error);
	}
}

}
