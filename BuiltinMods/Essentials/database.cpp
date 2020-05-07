#include "pch.h"

#include "global.h"
#include <sqlite3.h>
#include <boost/scope_exit.hpp>

std::unique_ptr<SQLite::Database> database;
std::unique_ptr<SQLite::Database> world_database;

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
}
template <> void migrateDatabase<1, 2>() { database->exec("ALTER TABLE custom_name ADD COLUMN nametag TEXT"); }
template <> void migrateDatabase<0, 2>() {
  migrateDatabase<0, 1>();
  migrateDatabase<1, 2>();
}

void initDatabase() {
  auto exists = !!database->execAndGet("SELECT count(*) FROM sqlite_master WHERE name = 'custom_name'").getInt();
  if (exists) {
    auto version = database->execAndGet("PRAGMA user_version").getInt();
    SQLite::Transaction trans{*database};
    try {
      switch (version) {
      case 0: // upgrade custom_name (uuid_a uuid_b to uuid (blob))
        migrateDatabase<0, 2>();
        break;
      case 1: // upgrade custom_name (add nametag)
        migrateDatabase<1, 2>();
        break;
      case 2: break;
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
      "prefix TEXT, postfix TEXT, nametag TEXT)");
  database->exec("PRAGMA user_version = 2");
}

void initWorldDatabase(std::filesystem::path const &base) {
  auto real      = base / settings.worldDatabase;
  world_database = std::make_unique<SQLite::Database>(real.string(), SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
  world_database->exec(
      "CREATE TABLE IF NOT EXISTS warp ("
      "uuid BLOB NOT NULL, "
      "name TEXT NOT NULL, "
      "dim INTEGER, "
      "x INTEGER, "
      "y INTEGER, "
      "z INTEGER, "
      "PRIMARY KEY (uuid, name))");
  world_database->exec(
      "CREATE TABLE IF NOT EXISTS global_warp ("
      "name TEXT NOT NULL PRIMARY KEY, "
      "dim INTEGER, "
      "x INTEGER, "
      "y INTEGER, "
      "z INTEGER)");
}