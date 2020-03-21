#include <SQLiteCpp/SQLiteCpp.h>

#include "global.h"

void InitDatabase() {
  db = std::make_unique<SQLite::Database>(settings.database, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
  db->exec(
      "CREATE TABLE IF NOT EXISTS transaction_log "
      "(uuid BLOB, value INTEGER, reason TEXT, time DATETIME DEFAULT CURRENT_TIMESTAMP)");
  db->exec(
      "CREATE VIEW IF NOT EXISTS balance (uuid, value) "
      "AS SELECT uuid, sum(value) FROM transaction_log "
      "GROUP BY uuid");
  db->exec(
      "CREATE TRIGGER IF NOT EXISTS checker BEFORE INSERT ON transaction_log "
      "FOR EACH ROW "
      "WHEN (SELECT value + NEW.value FROM balance WHERE uuid=NEW.uuid) < 0 "
      "BEGIN "
      "SELECT RAISE (FAIL, \"Prohibition of negative balance\"); "
      "END");
}