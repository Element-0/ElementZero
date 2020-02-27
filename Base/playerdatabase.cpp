#include <playerdb.h>
#include <log.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <sqlite3.h>
#include <Core/ExtendedCertificate.h>
#include <RakNet/RakPeer.h>
#include <mutex>
#include <boost/scope_exit.hpp>

#include "settings.hpp"

DEF_LOGGER("PlayerDB");

template <uint64_t sig> static void (Mod::PlayerDatabase::*emitter)(sigt<sig>, Mod::PlayerEntry const &);

static Mod::PlayerEntryContainer *container;
Mod::PlayerDatabase &db = Mod::PlayerDatabase::GetInstance();
std::unique_ptr<SQLite::Database> sqldb;

Mod::PlayerDatabase::PlayerDatabase() {
  emitter<"joined"_sig> = &Mod::PlayerDatabase::Emit;
  emitter<"left"_sig>   = &Mod::PlayerDatabase::Emit;
  container             = &data;
  sqldb = std::make_unique<SQLite::Database>(settings.UserDatabase, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
  sqldb->exec(
      "CREATE TABLE IF NOT EXISTS user"
      "(uuid UUID PRIMARY KEY, "
      "xuid INTEGER, "
      "name TEXT COLLATE NOCASE)");
  sqldb->exec(
      "CREATE TABLE IF NOT EXISTS login"
      "(uuid UUID, "
      "address TEXT, "
      "time INTEGER DEFAULT CURRENT_TIMESTAMP)");
  sqldb->exec(
      "CREATE TABLE IF NOT EXISTS logout"
      "(uuid UUID, "
      "time INTEGER DEFAULT CURRENT_TIMESTAMP)");
}

Mod::PlayerDatabase &Mod::PlayerDatabase::GetInstance() {
  static Mod::PlayerDatabase db;
  return db;
}

std::optional<Mod::PlayerEntry> Mod::PlayerDatabase::Find(Player *player) {
  auto &view = data.get<Player *>();
  if (auto it = view.find(player); it != view.end()) return *it;
  return {};
}
std::optional<Mod::PlayerEntry> Mod::PlayerDatabase::Find(std::string const &name) {
  auto &view = data.get<std::string>();
  if (auto it = view.find(name); it != view.end()) return *it;
  return {};
}
std::optional<Mod::PlayerEntry> Mod::PlayerDatabase::Find(uint64_t xuid) {
  auto &view = data.get<uint64_t>();
  if (auto it = view.find(xuid); it != view.end()) return *it;
  return {};
}
std::optional<Mod::PlayerEntry> Mod::PlayerDatabase::Find(mce::UUID const &uuid) {
  auto &view = data.get<mce::UUID>();
  if (auto it = view.find(uuid); it != view.end()) return *it;
  return {};
}
std::optional<Mod::OfflinePlayerEntry> Mod::PlayerDatabase::FindOffline(std::string const &name) {
  static SQLite::Statement find_by_name{*sqldb, "SELECT * FROM user WHERE name=? COLLATE NOCASE"};
  BOOST_SCOPE_EXIT_ALL() {
    find_by_name.reset();
    find_by_name.clearBindings();
  };
  find_by_name.bindNoCopy(1, name);
  if (find_by_name.executeStep()) {
    auto &uuid = *(mce::UUID *) find_by_name.getColumn(0).getBlob();
    auto xuid  = (uint64_t) find_by_name.getColumn(1).getInt64();
    return {{name, xuid, uuid}};
  }
  return {};
}
std::optional<Mod::OfflinePlayerEntry> Mod::PlayerDatabase::FindOffline(uint64_t xuid) {
  static SQLite::Statement find_by_xuid{*sqldb, "SELECT * FROM user WHERE xuid=?"};
  BOOST_SCOPE_EXIT_ALL() {
    find_by_xuid.reset();
    find_by_xuid.clearBindings();
  };
  find_by_xuid.bind(1, (int64_t) xuid);
  if (find_by_xuid.executeStep()) {
    auto &uuid = *(mce::UUID *) find_by_xuid.getColumn(0).getBlob();
    auto name  = find_by_xuid.getColumn(2).getString();
    return {{name, xuid, uuid}};
  }
  return {};
}
std::optional<Mod::OfflinePlayerEntry> Mod::PlayerDatabase::FindOffline(mce::UUID const &uuid) {
  static SQLite::Statement find_by_uuid{*sqldb, "SELECT * FROM user WHERE uuid=?"};
  BOOST_SCOPE_EXIT_ALL() {
    find_by_uuid.reset();
    find_by_uuid.clearBindings();
  };
  find_by_uuid.bindNoCopy(1, uuid, sizeof uuid);
  if (find_by_uuid.executeStep()) {
    auto xuid = (uint64_t) find_by_uuid.getColumn(1).getInt64();
    auto name  = find_by_uuid.getColumn(2).getString();
    return {{name, xuid, uuid}};
  }
  return {};
}

TClasslessInstanceHook(
    Player *,
    "?_createNewPlayer@ServerNetworkHandler@@AEAAAEAVServerPlayer@@AEBVNetworkIdentifier@@AEBVConnectionRequest@@@Z",
    NetworkIdentifier *id, void *req) {

  auto player  = original(this, id, req);
  auto &cert   = player->getCertificate();
  auto uuid    = ExtendedCertificate::getIdentity(cert);
  auto name    = ExtendedCertificate::getIdentityName(cert);
  auto xuid    = std::stoull(ExtendedCertificate::getXuid(cert));
  auto sadd    = id->getRealAddress();
  auto address = sadd.ToString();
  LOGV("%s joined (from %d)") % name % address;
  auto ref = container->emplace(Mod::PlayerEntry{player, name, xuid, uuid, *id});
  (db.*emitter<"joined"_sig>) (SIG("joined"), *ref.first);
  static SQLite::Statement stmt_user{*sqldb, "INSERT OR REPLACE INTO user VALUES (?, ?, ?)"};
  static SQLite::Statement stmt_login{*sqldb, "INSERT INTO login (uuid, address) VALUES (?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt_user.reset();
    stmt_user.clearBindings();
  };
  stmt_user.bindNoCopy(1, uuid, sizeof uuid);
  stmt_user.bind(2, (int64_t) xuid);
  stmt_user.bindNoCopy(3, name);
  stmt_user.exec();
  BOOST_SCOPE_EXIT_ALL() {
    stmt_login.reset();
    stmt_login.clearBindings();
  };
  stmt_login.bindNoCopy(1, uuid, sizeof uuid);
  stmt_login.bindNoCopy(2, address);
  stmt_login.exec();
  return player;
}

TClasslessInstanceHook(
    void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", Player *player, bool flag) {
  auto it = container->find(player);
  if (it != container->end()) {
    LOGV("%s left") % it->name;
    static SQLite::Statement stmt_logout{*sqldb, "INSERT INTO logout (uuid) VALUES (?)"};
    BOOST_SCOPE_EXIT_ALL() {
      stmt_logout.reset();
      stmt_logout.clearBindings();
    };
    stmt_logout.bindNoCopy(1, it->uuid, sizeof(mce::UUID));
    stmt_logout.exec();
    (db.*emitter<"left"_sig>) (SIG("left"), *it);
    container->erase(it);
  }
  original(this, player, flag);
}