#include <mutex>

#include <boost/scope_exit.hpp>
#include <boost/lexical_cast.hpp>

#include <RakNet/RakPeer.h>
#include <Actor/ServerPlayer.h>
#include <Net/NetworkIdentifier.h>
#include <Core/ExtendedCertificate.h>

#include <SQLiteCpp/SQLiteCpp.h>
#include <sqlite3.h>

#include <modutils.h>
#include <hook.h>

#include <base/playerdb.h>
#include <base/log.h>

#include "boost/lexical_cast.hpp"
#include "settings.hpp"

DEF_LOGGER("PlayerDB");

template <SIGTEMPLATE> static void (Mod::PlayerDatabase::*emitter)(sigt<sig>, Mod::PlayerEntry const &);
static void (Mod::PlayerDatabase::*emit_change_dim)(
    sigt<"change_dimension"_sig>, Mod::PlayerEntry const &, ChangeDimensionRequest const &, bool stage);

static Mod::PlayerEntryContainer *container;
static std::map<Player *, std::map<std::type_index, std::shared_ptr<Mod::AuxHolder>>> *auxm;
Mod::PlayerDatabase &db = Mod::PlayerDatabase::GetInstance();
std::unique_ptr<SQLite::Database> sqldb;

Mod::PlayerDatabase::PlayerDatabase() {
  emitter<"joined"_sig>      = &Mod::PlayerDatabase::Emit;
  emitter<"initialized"_sig> = &Mod::PlayerDatabase::Emit;
  emitter<"left"_sig>        = &Mod::PlayerDatabase::Emit;
  emit_change_dim            = &Mod::PlayerDatabase::Emit;
  container                  = &data;
  auxm                       = &auxmap;
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
      "time INTEGER DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'now')))");
  sqldb->exec(
      "CREATE TABLE IF NOT EXISTS logout"
      "(uuid UUID, "
      "time INTEGER DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'now')))");
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
std::optional<Mod::PlayerEntry> Mod::PlayerDatabase::Find(NetworkIdentifier const &netid) {
  auto &view = data.get<NetworkIdentifier>();
  if (auto it = view.find(netid); it != view.end()) return *it;
  return {};
}
std::optional<Mod::PlayerEntry> Mod::PlayerDatabase::GeneralFind(std::string const &target) {
  if (target.size() == 36) {
    if (auto uuid = mce::UUID::fromString(target); !uuid.empty()) { return db.Find(uuid); }
  }
  try {
    auto xuid = boost::lexical_cast<uint64_t>(target);
    return db.Find(xuid);
  } catch (boost::bad_lexical_cast const &bad) {}
  return Find(target);
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
    auto name = find_by_uuid.getColumn(2).getString();
    return {{name, xuid, uuid}};
  }
  return {};
}
std::optional<Mod::OfflinePlayerEntry> Mod::PlayerDatabase::GeneralFindOffline(std::string const &target) {
  if (target.size() == 36) {
    if (auto uuid = mce::UUID::fromString(target); !uuid.empty()) { return db.FindOffline(uuid); }
  }
  try {
    auto xuid = boost::lexical_cast<uint64_t>(target);
    return db.FindOffline(xuid);
  } catch (boost::bad_lexical_cast const &bad) {}
  return FindOffline(target);
}

TClasslessInstanceHook(
    std::unique_ptr<ServerPlayer>,
    "?createNewPlayer@ServerNetworkHandler@@QEAA?AV?$unique_ptr@VServerPlayer@@U?$default_delete@VServerPlayer@@@std@@@"
    "std@@AEBVNetworkIdentifier@@AEBVConnectionRequest@@@Z",
    NetworkIdentifier *id, void *req) {
  auto player  = original(this, id, req);
  auto &cert   = player->getCertificate();
  auto uuid    = ExtendedCertificate::getIdentity(cert);
  auto name    = ExtendedCertificate::getIdentityName(cert);
  auto sadd    = id->getRealAddress();
  auto address = sadd.ToString();
  try {
    auto xuid = std::stoull(ExtendedCertificate::getXuid(cert));
    LOGV("%s joined (from %s)") % name % address;
    auto ref = container->emplace(Mod::PlayerEntry{player.get(), name, xuid, uuid, *id});
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
  } catch (...) {
    LOGV("illegal connection from %s (name: %s)") % address % name;
    player->kick();
  }
  return std::move(player);
}

TClasslessInstanceHook(
    void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", Player *player, bool flag) {
  if (auto it = container->find(player); it != container->end()) {
    LOGV("%s left") % it->name;
    static SQLite::Statement stmt_logout{*sqldb, "INSERT INTO logout (uuid) VALUES (?)"};
    BOOST_SCOPE_EXIT_ALL() {
      stmt_logout.reset();
      stmt_logout.clearBindings();
    };
    stmt_logout.bindNoCopy(1, it->uuid, sizeof(mce::UUID));
    stmt_logout.exec();
    (db.*emitter<"left"_sig>) (SIG("left"), *it);
    auxm->erase(player);
    container->erase(it);
    original(this, player, flag);
  }
}

TClasslessInstanceHook(
    bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z", Player *player,
    ChangeDimensionRequest &request) {
  bool ret = false;
  if (request.unk0 == 0) {
    if (auto it = container->find(player); it != container->end()) {
      (db.*emit_change_dim)(SIG("change_dimension"), *it, request, false);
      ret = original(this, player, request);
    }
  } else {
    ret = original(this, player, request);
    if (ret)
      if (auto it = container->find(player); it != container->end())
        (db.*emit_change_dim)(SIG("change_dimension"), *it, request, true);
  }
  return ret;
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
    NetworkIdentifier const &netid, void *packet) {
  original(this, netid, packet);
  auto &view = container->get<NetworkIdentifier>();
  if (auto it = view.find(netid); it != view.end()) (db.*emitter<"initialized"_sig>) (SIG("initialized"), *it);
}