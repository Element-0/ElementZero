#include "settings.h"

std::unique_ptr<SQLite::Database> database;

void InitDatabase() {
  database = std::make_unique<SQLite::Database>(settings.Database, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
  database->exec(
      "CREATE TABLE IF NOT EXISTS audit_action ("
      "time INTEGER DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'now')), "
      "session BLOB, "
      "player BLOB, "
      "dimension INTEGER, "
      "blocked TEXT, "
      "type INTEGER, "
      "x INTEGER, "
      "y INTEGER, "
      "z INTEGER, "
      "face INTEGER)");
  database->exec(
      "CREATE TABLE IF NOT EXISTS audit_transaction ("
      "time INTEGER DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'now')), "
      "session BLOB, "
      "player BLOB, "
      "dimension INTEGER, "
      "blocked TEXT, "
      "data BLOB)");
  database->exec("PRAGMA journal_mode = WAL");
  database->exec("PRAGMA synchronous = NORMAL");
}