#include <memory>

#include <SQLiteCpp/Database.h>

#include <log.h>

#include "global.h"

template <int from, int to> void migrateDatabase();

template <> void migrateDatabase<0, 1>() {
  database->exec(
      "CREATE TABLE IF NOT EXISTS packets ("
      "session BLOB CHECK(length(session) = 16), "
      "netid INT, "
      "time DATETIME DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'now')), "
      "type BOOLEAN CHECK(type = 0 OR type = 1), "
      "data BLOB)");
  database->exec(
      "INSERT INTO packets (session, netid, time, type, data)"
      "SELECT session, netid, time, type, data FROM ("
      "SELECT rowid, session, netid, time, 0 as type, data FROM record UNION "
      "SELECT rowid, session, netid, time, 1 as type, data FROM send_record) "
      "ORDER BY time, rowid");
  database->exec("DROP TABLE record");
  database->exec("DROP TABLE send_record");
  database->exec("PRAGMA user_version = 1");
}

DEF_LOGGER("PacketDumper");

void InitDatabase() {
  database = std::make_unique<SQLite::Database>(settings.Database, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  database->exec("PRAGMA journal_mode = WAL");
  database->exec("PRAGMA synchronous = NORMAL");
  auto exists = !!database->execAndGet("SELECT count(*) FROM sqlite_master WHERE name = 'idmap'").getInt();
  if (exists) {
    auto version = database->execAndGet("PRAGMA user_version").getInt();
    SQLite::Transaction trans{*database};
    try {
      switch (version) {
      case 0: // upgrade custom_name (uuid_a uuid_b to uuid (blob))
        migrateDatabase<0, 1>();
        break;
      case 1: break;
      default: {
        LOGE("database corrupted");
        exit(1);
        break;
      }
      }
      trans.commit();
    } catch (SQLite::Exception const &sqle) {
      DEF_LOGGER("essentials");
      LOGE("exception: %s") % sqle.what();
      exit(1);
    }
  }
  database->exec(
      "CREATE TABLE IF NOT EXISTS idmap ("
      "session BLOB CHECK(length(session) = 16), "
      "netid INT, "
      "address TEXT, "
      "xuid INT, "
      "PRIMARY KEY(session, netid))");
  database->exec(
      "CREATE TABLE IF NOT EXISTS packets ("
      "session BLOB CHECK(length(session) = 16), "
      "netid INT, "
      "time DATETIME DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'now')), "
      "type BOOLEAN CHECK(type = 0 OR type = 1), "
      "data BLOB)");
  database->exec("PRAGMA user_version = 1");
}