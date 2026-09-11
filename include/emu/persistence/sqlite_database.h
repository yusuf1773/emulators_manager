#pragma once

#include <filesystem>

struct sqlite3;

namespace emu {

class SqliteDatabase {
public:
	explicit SqliteDatabase(const std::filesystem::path& path);
	~SqliteDatabase();

	SqliteDatabase(const SqliteDatabase&) = delete;
	SqliteDatabase& operator=(const SqliteDatabase&) = delete;

	sqlite3* handle() const { return database_; }

private:
	void Execute(const char* sql);
	void Migrate();

	sqlite3* database_ = nullptr;
};

}
