#include <string>

#include <SQLiteCpp/Statement.h>

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

void LogPacket(NetworkIdentifier id, std::string const &data) {
  static SQLite::Statement cache{*database, "INSERT INTO record VALUES (?, ?, CURRENT_TIMESTAMP, ?)"};
  static auto sess = SessionUUID();
  BOOST_SCOPE_EXIT_ALL() {
    cache.reset();
    cache.clearBindings();
  };
  cache.bindNoCopy(1, sess, sizeof sess);
  cache.bind(2, (int64_t) id.guid.g);
  cache.bindNoCopy(3, data);
  cache.exec();
}

THook(
    int,
    "?receivePacket@Connection@NetworkHandler@@QEAA?AW4DataStatus@NetworkPeer@@AEAV?$basic_string@DU?$char_traits@D@"
    "std@@V?$allocator@D@2@@std@@@Z",
    NetworkHandler::Connection *self, std::string &data) {
  auto status = original(self, data);
  if (status == 0 && database) LogPacket(self->id, data);
  return status;
}
