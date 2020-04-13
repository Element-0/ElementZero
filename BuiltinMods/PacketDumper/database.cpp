#include <memory>

#include <SQLiteCpp/Database.h>

#include "global.h"

void InitDatabase() {
  database = std::make_unique<SQLite::Database>(settings.Database, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  database->exec(
      "CREATE TABLE IF NOT EXISTS idmap ("
      "session BLOB, "
      "netid BLOB, "
      "address TEXT, "
      "xuid INT, "
      "PRIMARY KEY(session, netid))");
  database->exec(
      "CREATE TABLE IF NOT EXISTS record ("
      "session BLOB, "
      "netid INT, "
      "time DATETIME, "
      "data BLOB)");
  database->exec("PRAGMA journal_mode = WAL");
  database->exec("PRAGMA synchronous = NORMAL");
}