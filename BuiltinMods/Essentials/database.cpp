#include "pch.h"

#include "global.h"
#include <sqlite3.h>
#include <boost/scope_exit.hpp>

template <int from, int to> void migrateDatabase();

template <> void migrateDatabase<0, 1>() {
  database->exec("ALTER TABLE custom_name RENAME TO custom_name_bak_0");
  database->createFunction(
      "make_uuid", 2, true, nullptr,
      [](sqlite3_context *ctx, int argc, sqlite3_value **argv) {
        auto uuid = new mce::UUID;
        uuid->a   = (uint64_t) sqlite3_value_int64(argv[0]);
        uuid->b   = (uint64_t) sqlite3_value_int64(argv[1]);
        sqlite3_result_blob(ctx, uuid, sizeof *uuid, [](void *ptr) { delete (mce::UUID *) ptr; });
      },
      nullptr, nullptr, nullptr);
  database->exec(
      "CREATE TABLE custom_name ("
      "uuid BLOB NOT NULL PRIMARY KEY, "
      "prefix TEXT, postfix TEXT)");
  database->exec(
      "INSERT INTO custom_name (uuid, prefix, postfix) "
      "SELECT make_uuid(uuid_a, uuid_b), "
      "prefix, postfix "
      "FROM custom_name_bak_0");
  database->exec("PRAGMA user_version = 1");
}

void initDatabase() {
  auto exists = !!database->execAndGet("SELECT count(*) FROM sqlite_master WHERE name = 'custom_name'").getInt();
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
        DEF_LOGGER("essentials");
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
      "CREATE TABLE IF NOT EXISTS custom_name ("
      "uuid BLOB NOT NULL PRIMARY KEY, "
      "prefix TEXT, postfix TEXT)");
  database->exec(
      "CREATE TABLE IF NOT EXISTS chat ("
      "uuid BLOB NOT NULL, "
      "name TEXT, "
      "content TEXT, "
      "time INTEGER DEFAULT CURRENT_TIMESTAMP)");
}