#include "emu/persistence/sqlite_database.h"

#include <sqlite3.h>

#include <stdexcept>
#include <string>

namespace emu {

SqliteDatabase::SqliteDatabase(const std::filesystem::path& path) {
	if (!path.parent_path().empty()) {
		std::filesystem::create_directories(path.parent_path());
	}
	if (sqlite3_open_v2(path.string().c_str(), &database_,
						SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
						nullptr) != SQLITE_OK) {
		const std::string message = database_ == nullptr
										? "Cannot open SQLite database"
										: sqlite3_errmsg(database_);
		if (database_ != nullptr) sqlite3_close(database_);
		database_ = nullptr;
		throw std::runtime_error(message);
	}
	sqlite3_busy_timeout(database_, 5000);
	Execute("PRAGMA foreign_keys = ON");
	Migrate();
}

SqliteDatabase::~SqliteDatabase() {
	if (database_ != nullptr) sqlite3_close(database_);
}

void SqliteDatabase::Execute(const char* sql) {
	char* error = nullptr;
	if (sqlite3_exec(database_, sql, nullptr, nullptr, &error) != SQLITE_OK) {
		const std::string message =
			error == nullptr ? sqlite3_errmsg(database_) : error;
		sqlite3_free(error);
		throw std::runtime_error(message);
	}
}

void SqliteDatabase::Migrate() {
	sqlite3_stmt* statement = nullptr;
	if (sqlite3_prepare_v2(database_, "PRAGMA user_version", -1, &statement,
						   nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(database_));
	}
	const int step = sqlite3_step(statement);
	const int version =
		step == SQLITE_ROW ? sqlite3_column_int(statement, 0) : -1;
	sqlite3_finalize(statement);
	if (version < 0) {
		throw std::runtime_error("Cannot read SQLite schema version");
	}
	if (version > 1) {
		throw std::runtime_error("Unsupported SQLite schema version: " +
								 std::to_string(version));
	}
	if (version == 1) return;

	Execute("BEGIN IMMEDIATE");
	try {
		Execute(
			"CREATE TABLE IF NOT EXISTS games("
			"id TEXT PRIMARY KEY,"
			"name TEXT NOT NULL UNIQUE COLLATE NOCASE,"
			"content_path TEXT NOT NULL,"
			"platform TEXT NOT NULL)");
		Execute(
			"CREATE TABLE IF NOT EXISTS emulators("
			"id TEXT PRIMARY KEY,"
			"name TEXT NOT NULL UNIQUE COLLATE NOCASE,"
			"executable_path TEXT NOT NULL,"
			"platform TEXT NOT NULL,"
			"base_arguments TEXT NOT NULL)");
		Execute(
			"CREATE TABLE IF NOT EXISTS launch_profiles("
			"id TEXT PRIMARY KEY,"
			"name TEXT NOT NULL,"
			"game_id TEXT NOT NULL REFERENCES games(id) ON DELETE CASCADE,"
			"emulator_id TEXT NOT NULL REFERENCES emulators(id) ON DELETE "
			"CASCADE,"
			"strategy_id TEXT NOT NULL,"
			"arguments TEXT NOT NULL,"
			"working_directory TEXT NOT NULL,"
			"is_default INTEGER NOT NULL CHECK(is_default IN (0,1)),"
			"UNIQUE(game_id, name COLLATE NOCASE))");
		Execute(
			"CREATE UNIQUE INDEX IF NOT EXISTS one_default_profile_per_game "
			"ON launch_profiles(game_id) WHERE is_default = 1");
		Execute(
			"CREATE TABLE IF NOT EXISTS command_history("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"command TEXT NOT NULL,"
			"timestamp INTEGER NOT NULL)");
		Execute("PRAGMA user_version = 1");
		Execute("COMMIT");
	} catch (...) {
		try {
			Execute("ROLLBACK");
		} catch (...) {
		}
		throw;
	}
}

}
