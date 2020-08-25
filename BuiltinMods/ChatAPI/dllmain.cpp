#include <string>
#include <memory>

#include <boost/scope_exit.hpp>

#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include <Packet/TextPacket.h>
#include <Actor/Player.h>
#include <Level/Level.h>

#include <hook.h>
#include <dllentry.h>

#include <base/playerdb.h>
#include <base/log.h>
#include <mods/ChatAPI.h>

void dllenter() {}
void dllexit() {}

static std::unique_ptr<SQLite::Database> database;

static void (Mod::Chat::*emitter)(
    sigt<"chat"_sig>, Mod::PlayerEntry const &, std::string &, std::string &, Mod::CallbackToken<std::string> &);

namespace Mod {

Chat::Chat() { emitter = &Chat::Emit; }

void Chat::SendBroadcast(const std::string &name, const std::string &content) {
  auto packet = TextPacket::createTextPacket<TextPacketType::Chat>(name, content, "");
  LocateService<Level>()->forEachPlayer([&](Player const &p) -> bool {
    p.sendNetworkPacket(packet);
    return true;
  });
}

void Chat::SendAnnounce(const std::string &content) {
  auto packet = TextPacket::createTextPacket<TextPacketType::Announcement>(content);
  LocateService<Level>()->forEachPlayer([&](Player const &p) -> bool {
    p.sendNetworkPacket(packet);
    return true;
  });
}

void Chat::SendAnnounce(const std::string &content, std::initializer_list<std::string> args) {
  auto packet = TextPacket::createTranslatedMessageWithParams(content, args);
  LocateService<Level>()->forEachPlayer([&](Player const &p) -> bool {
    p.sendNetworkPacket(packet);
    return true;
  });
}

void Chat::Send(const Mod::PlayerEntry &entry, const std::string &content) {
  auto packet = TextPacket::createTextPacket<TextPacketType::Announcement>(content);
  entry.player->sendNetworkPacket(packet);
}

void Chat::Send(const Mod::PlayerEntry &entry, const std::string &content, std::initializer_list<std::string> args) {
  auto packet = TextPacket::createTranslatedMessageWithParams(content, args);
  entry.player->sendNetworkPacket(packet);
}

Chat &Chat::GetInstance() {
  static Chat instance;
  return instance;
}

} // namespace Mod

static struct Settings {
  std::string database = "chat.db";

  struct CustomChatFormatting {
    bool enabled       = false;
    std::string prefix = "<";
    std::string sufix  = ">";

    template <typename IO> static inline bool io(IO f, CustomChatFormatting &self, YAML::Node &node) {
      return f(self.enabled, node["enabled"]) && f(self.prefix, node["prefix"]) && f(self.sufix, node["sufix"]);
    }
  } customChatFormatting;

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.database, node["database"]) && f(settings.customChatFormatting, node["customChatFormatting"]);
  }
} settings;

DEFAULT_SETTINGS(settings);

void PreInit() {
  database = std::make_unique<SQLite::Database>(settings.database, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
  database->exec("PRAGMA journal_mode = WAL");
  database->exec("PRAGMA synchronous = NORMAL");
  database->exec(
      "CREATE TABLE IF NOT EXISTS chat ("
      "uuid BLOB NOT NULL, "
      "name TEXT, "
      "content TEXT, "
      "time INTEGER DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'now')))");
}

void static logChat(Mod::PlayerEntry const &entry, std::string const &content) {
  if (!database) return;
  DEF_LOGGER("CHAT");
  LOGI("[%s] %s") % entry.name % content;
  static SQLite::Statement stmt{*database, "INSERT INTO chat (uuid, name, content) VALUES (?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.reset();
    stmt.clearBindings();
  };
  stmt.bindNoCopy(1, entry.uuid, sizeof entry.uuid);
  stmt.bindNoCopy(2, entry.name);
  stmt.bindNoCopy(3, content);
  stmt.exec();
}

TClasslessInstanceHook(
    void,
    "?_displayGameMessage@ServerNetworkHandler@@AEAAXAEBVPlayer@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$"
    "allocator@D@2@@std@@@Z",
    Player *player, std::string &content) try {
  DEF_LOGGER("CHAT");
  auto &playerdb = Mod::PlayerDatabase::GetInstance().GetData();
  auto it        = playerdb.find(player);
  if (it == playerdb.end()) return;
  std::string displayName = it->name;
  Mod::CallbackToken<std::string> token;
  (Mod::Chat::GetInstance().*emitter)(SIG("chat"), *it, displayName, content, token);
  if (token) {
    auto packet = TextPacket::createTranslatedMessageWithParams("chat.blocked", {*token});
    player->sendNetworkPacket(packet);
    return;
  }
  TextPacket packet;
  if (!settings.customChatFormatting.enabled) {
    packet = TextPacket::createTextPacket<TextPacketType::Chat>(displayName, content, std::to_string(it->xuid));
  } else {
    packet = TextPacket::createTextPacket<TextPacketType::SystemMessage>(displayName, settings.customChatFormatting.prefix + displayName + settings.customChatFormatting.sufix + " " + content, std::to_string(it->xuid));
  }
  LocateService<Level>()->forEachPlayer([&](Player const &p) -> bool {
    p.sendNetworkPacket(packet);
    return true;
  });
  logChat(*it, content);
} catch (...) {}