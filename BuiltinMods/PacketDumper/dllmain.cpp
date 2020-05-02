#include <string>

#include <SQLiteCpp/Statement.h>

#include <Core/Packet.h>
#include <Net/NetworkPeer.h>
#include <Net/NetworkIdentifier.h>
#include <Net/NetworkHandler.h>

#include <dllentry.h>
#include <hook.h>
#include <playerdb.h>
#include <yaml.h>
#include <base.h>

#include "boost/scope_exit.hpp"
#include "global.h"

DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

void PreInit() {
  InitDatabase();
  Mod::PlayerDatabase::GetInstance().AddListener(SIG("joined"), [](Mod::PlayerEntry const &entry) {
    static SQLite::Statement cache{*database, "INSERT OR REPLACE INTO idmap VALUES (?, ?, ?, ?)"};
    static auto sess = SessionUUID();
    BOOST_SCOPE_EXIT_ALL() {
      cache.reset();
      cache.clearBindings();
    };
    cache.bindNoCopy(1, sess, sizeof sess);
    cache.bind(2, (int64_t) entry.netid.guid.g);
    cache.bind(3, entry.netid.getRealAddress().ToString());
    cache.bind(4, (int64_t) entry.xuid);
    cache.exec();
  });
}

void LogPacket(bool isSent, NetworkIdentifier id, std::string const &data) {
  static SQLite::Statement cache{*database, "INSERT INTO packets (session, netid, type, data) VALUES (?, ?, ?, ?)"};
  static auto sess = SessionUUID();
  BOOST_SCOPE_EXIT_ALL() {
    cache.reset();
    cache.clearBindings();
  };
  cache.bindNoCopy(1, sess, sizeof sess);
  cache.bind(2, (int64_t) id.guid.g);
  cache.bind(3, isSent);
  cache.bindNoCopy(4, data);
  cache.exec();
}

TInstanceHook(
    NetworkPeer::DataStatus,
    "?receivePacket@Connection@NetworkHandler@@QEAA?AW4DataStatus@NetworkPeer@@AEAV?$basic_string@DU?$char_traits@D@"
    "std@@V?$allocator@D@2@@std@@@Z",
    NetworkHandler::Connection, std::string &data) {
  auto status = original(this, data);
  if (status == NetworkPeer::DataStatus::OK && database) LogPacket(false, id, data);
  return status;
}

TClasslessInstanceHook(
    void,
    "?_sendInternal@NetworkHandler@@AEAAXAEBVNetworkIdentifier@@AEBVPacket@@AEBV?$basic_string@DU?$char_traits@D@std@@"
    "V?$allocator@D@2@@std@@@Z",
    NetworkIdentifier const &id, Packet const &pkt, std::string const &content) {
  LogPacket(true, id, content);
  original(this, id, pkt, content);
}
