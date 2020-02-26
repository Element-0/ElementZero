#include <vector>
#include <string>
#include <optional>
#include <boost/scope_exit.hpp>
#include <boost/format.hpp>

#include <hook.h>
#include <log.h>
#include <dllentry.h>

#include <Core/mce.h>
#include <Net/NetworkIdentifier.h>

#include "settings.h"

DEF_LOGGER("Blacklist");
Settings settings;
std::unique_ptr<SQLite::Database> database;

DEFAULT_SETTINGS(settings);

void PreInit() {
  LOGV("Initialize database %s") % settings.Database;
  database = std::make_unique<SQLite::Database>(settings.Database, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  database->exec(
      "CREATE TABLE IF NOT EXISTS uuid("
      "value BLOB PRIMARY KEY, "
      "name TEXT, "
      "reason TEXT NOT NULL, "
      "source TEXT, "
      "time INTEGER DEFAULT CURRENT_TIMESTAMP)");
  database->exec(
      "CREATE TABLE IF NOT EXISTS xuid("
      "value INTEGER PRIMARY KEY, "
      "name TEXT, "
      "reason TEXT NOT NULL, "
      "source TEXT, "
      "time INTEGER DEFAULT CURRENT_TIMESTAMP)");
  database->exec(
      "CREATE TABLE IF NOT EXISTS name("
      "value TEXT PRIMARY KEY, "
      "reason TEXT NOT NULL, "
      "source TEXT, "
      "time INTEGER DEFAULT CURRENT_TIMESTAMP)");
  database->exec(
      "CREATE TABLE IF NOT EXISTS iplog("
      "uuid BLOB NOT NULL, "
      "xuid INTEGER, "
      "name TEXT NOT NULL, "
      "reason TEXT NOT NULL, "
      "address TEXT NOT NULL, "
      "time INTEGER DEFAULT CURRENT_TIMESTAMP)");
}

void dllenter() {}
void dllexit() {}

struct BlacklistEntry {
  mce::UUID uuid;
  std::string xuid, reason;
  int duration;
};

static_assert(sizeof(BlacklistEntry) == 88);

static bool queryForName(std::string const &name, BlacklistEntry *&it) {
  static auto search_name = SQLite::Statement(*database, "SELECT reason FROM name WHERE value == ?");
  search_name.bindNoCopy(1, name);
  BOOST_SCOPE_EXIT_ALL() {
    search_name.reset();
    search_name.clearBindings();
  };
  if (search_name.executeStep()) {
    static BlacklistEntry cache;
    cache.reason = search_name.getColumn(0).getString();
    it           = &cache;
    return true;
  }
  return false;
}

static bool queryForXUID(int64_t xuid, std::string latest_name, BlacklistEntry *&it) {
  static auto search_xuid = SQLite::Statement(*database, "SELECT reason, name FROM xuid WHERE value == ?");
  static auto update_name = SQLite::Statement(*database, "UPDATE xuid SET name = ? WHERE value == ?");
  search_xuid.bind(1, xuid);
  BOOST_SCOPE_EXIT_ALL() {
    search_xuid.reset();
    search_xuid.clearBindings();
  };
  if (search_xuid.executeStep()) {
    static BlacklistEntry cache;
    cache.reason   = search_xuid.getColumn(0).getString();
    it             = &cache;
    auto last_name = search_xuid.getColumn(1);
    if (last_name.isNull() || last_name.getString() != latest_name) {
      update_name.bind(1, latest_name);
      update_name.bind(2, xuid);
      BOOST_SCOPE_EXIT_ALL() {
        update_name.reset();
        update_name.clearBindings();
      };
      update_name.exec();
    }
    return true;
  }
  return false;
}

static bool queryForUUID(char const *uuid, std::string latest_name, BlacklistEntry *&it) {
  static auto search_uuid = SQLite::Statement(*database, "SELECT reason, name FROM uuid WHERE value == ?");
  static auto update_name = SQLite::Statement(*database, "UPDATE uuid SET name = ? WHERE value == ?");
  search_uuid.bindNoCopy(1, uuid, sizeof(mce::UUID));
  BOOST_SCOPE_EXIT_ALL() {
    search_uuid.reset();
    search_uuid.clearBindings();
  };
  if (search_uuid.executeStep()) {
    static BlacklistEntry cache;
    cache.reason   = search_uuid.getColumn(0).getString();
    it             = &cache;
    auto last_name = search_uuid.getColumn(1);
    if (last_name.isNull() || last_name.getString() != latest_name) {
      update_name.bind(1, latest_name);
      update_name.bindNoCopy(2, uuid, sizeof(mce::UUID));
      BOOST_SCOPE_EXIT_ALL() {
        update_name.reset();
        update_name.clearBindings();
      };
      update_name.exec();
    }
    return true;
  }
  return false;
}

static void updateIDs(char const *uuid, int64_t xuid, std::string const &name, std::string const &reason) {
  static SQLite::Statement update_uuid{*database,
                                       "INSERT OR IGNORE INTO uuid "
                                       "(value, name, reason, source) "
                                       "VALUES (?, ?, ?, ?)"};
  static SQLite::Statement update_xuid{*database,
                                       "INSERT OR IGNORE INTO xuid "
                                       "(value, name, reason, source) "
                                       "VALUES (?, ?, ?, ?)"};
  auto source = (boost::format("GENERATED FROM NAME %s") % name).str();
  update_uuid.bindNoCopy(1, uuid, sizeof(mce::UUID));
  update_uuid.bindNoCopy(2, name);
  update_uuid.bindNoCopy(3, reason);
  update_uuid.bindNoCopy(4, source);
  BOOST_SCOPE_EXIT_ALL() {
    update_uuid.reset();
    update_uuid.clearBindings();
  };
  update_uuid.exec();
  update_xuid.bind(1, xuid);
  update_xuid.bindNoCopy(2, name);
  update_xuid.bindNoCopy(3, reason);
  update_xuid.bindNoCopy(4, source);
  BOOST_SCOPE_EXIT_ALL() {
    update_xuid.reset();
    update_xuid.clearBindings();
  };
  update_xuid.exec();
}

static void logIpAddress(
    char const *uuid, int64_t xuid, std::string const &name, std::string const &reason, std::string const &ipaddress) {
  static SQLite::Statement insert_log{*database,
                                      "INSERT INTO iplog ("
                                      "uuid, xuid, name, reason, address"
                                      ") VALUES ("
                                      "?, ?, ?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    insert_log.reset();
    insert_log.clearBindings();
  };
  insert_log.bindNoCopy(1, uuid, sizeof(mce::UUID));
  insert_log.bind(2, xuid);
  insert_log.bindNoCopy(3, name);
  insert_log.bindNoCopy(4, reason);
  insert_log.bindNoCopy(5, ipaddress);
  insert_log.exec();
}

static thread_local NetworkIdentifier *context;
static thread_local std::optional<std::pair<boolean, BlacklistEntry *>> cached;

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVLoginPacket@@@Z", NetworkIdentifier &id,
    void *packet) {
  context = &id;
  cached  = {};
  original(this, id, packet);
  context = nullptr;
}

TClasslessInstanceHook(
    bool,
    "?isBlocked@Blacklist@@AEBA_NAEBUEntry@1@AEAV?$_Vector_const_iterator@V?$_Vector_val@U?$_Simple_types@UEntry@"
    "Blacklist@@@std@@@std@@@std@@@Z",
    BlacklistEntry const &id, BlacklistEntry *&it) {
  if (cached) {
    if (cached->first) {
      it = cached->second;
      return true;
    }
    return false;
  }
  auto &name = *(std::string *) ((char *) &id - 64); // HACK
  auto xuid  = std::stoll(id.xuid);
  auto uuid  = (char const *) &id.uuid;
  if (queryForUUID(uuid, name, it) || queryForXUID(xuid, name, it)) goto logip;
  if (queryForName(name, it)) {
    updateIDs(uuid, xuid, name, it->reason);
    goto logip;
  }
  cached = {false, nullptr};
  return false;
logip:
  if (context) logIpAddress(uuid, xuid, name, it->reason, context->getRealAddress().ToString(false));
  cached = {true, it};
  return true;
}
