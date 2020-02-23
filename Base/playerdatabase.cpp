#include <playerdb.h>
#include <log.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <sqlite3.h>
#include <Core/ExtendedCertificate.h>
#include <RakNet/RakPeer.h>
#include <mutex>

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
      "name TEXT)");
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

TClasslessInstanceHook(
    Player *,
    "?_createNewPlayer@ServerNetworkHandler@@AEAAAEAVServerPlayer@@AEBVNetworkIdentifier@@AEBVConnectionRequest@@@Z",
    NetworkIdentifier *id, void *req) {

  auto player  = original(this, id, req);
  auto &cert   = player->getCertificate();
  auto uuid    = ExtendedCertificate::getIdentity(cert);
  auto name    = ExtendedCertificate::getIdentityName(cert);
  auto xuid    = std::stoull(ExtendedCertificate::getXuid(cert));
  auto sadd    = LocateService<RakNet::RakPeer>()->GetSystemAddressFromGuid(id->guid);
  auto address = sadd.ToString();
  LOGV("%s joined (from %d)") % name % address;
  auto ref = container->emplace(Mod::PlayerEntry{player, name, xuid, uuid, *id});
  (db.*emitter<"joined"_sig>) (SIG("joined"), *ref.first);
  static SQLite::Statement stmt_user{*sqldb, "INSERT OR REPLACE INTO user VALUES (?, ?, ?)"};
  static SQLite::Statement stmt_login{*sqldb, "INSERT INTO login (uuid, address) VALUES (?, ?)"};
  stmt_user.bindNoCopy(1, uuid, sizeof uuid);
  stmt_user.bind(2, (int64_t) xuid);
  stmt_user.bindNoCopy(3, name);
  stmt_user.exec();
  stmt_user.reset();
  stmt_user.clearBindings();
  stmt_login.bindNoCopy(1, uuid, sizeof uuid);
  stmt_login.bindNoCopy(2, address);
  stmt_login.exec();
  stmt_login.reset();
  stmt_login.clearBindings();
  return player;
}

TClasslessInstanceHook(
    void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", Player *player, bool flag) {
  auto it = container->find(player);
  if (it != container->end()) {
    LOGV("%s left") % it->name;
    static SQLite::Statement stmt_logout{*sqldb, "INSERT INTO logout (uuid) VALUES (?)"};
    stmt_logout.bindNoCopy(1, it->uuid, sizeof(mce::UUID));
    stmt_logout.exec();
    stmt_logout.reset();
    stmt_logout.clearBindings();
    (db.*emitter<"left"_sig>) (SIG("left"), *it);
    container->erase(it);
  }
  original(this, player, flag);
}