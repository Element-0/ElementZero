#include "pch.h"

#include <memory>
#include <boost/scope_exit.hpp>

#include <sqlite3.h>

#include <base/base.h>
#include <base/playerdb.h>
#include <mods/ChatAPI.h>

#include "global.h"

void ChatHandler(
    Mod::PlayerEntry const &entry, std::string &displayName, std::string &content,
    Mod::CallbackToken<std::string> &token) {
  if (token) return;
  static SQLite::Statement stmt{*database, "SELECT prefix, postfix FROM custom_name WHERE uuid = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.reset();
    stmt.clearBindings();
  };
  stmt.bindNoCopy(1, entry.uuid, sizeof entry.uuid);
  if (stmt.executeStep()) {
    char const *prefix  = stmt.getColumn("prefix");
    char const *postfix = stmt.getColumn("postfix");
    displayName         = (boost::format("%s%s%s") % prefix % displayName % postfix).str();
  }
}

enum class Action { Set, Clear };

class SetCustomNameCommand : public Command {
public:
  Action _sig;
  enum class Key { Prefix, Postfix } key;
  CommandSelector<Player> selector;
  std::string str = "";
  SetCustomNameCommand() { selector.setIncludeDeadPlayers(true); }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    int count = 0;
    SQLite::Transaction transaction(*database);
    auto &playerdb = Mod::PlayerDatabase::GetInstance().GetData();
    for (auto player : selector.results(origin)) {
      auto it = playerdb.find(player);
      if (it != playerdb.end()) {
        try {
          static SQLite::Statement prefix_stmt{
              *database,
              "INSERT INTO custom_name (uuid, prefix) VALUES (?, ?) ON "
              "CONFLICT(uuid) DO UPDATE SET prefix = excluded.prefix"};
          static SQLite::Statement postfix_stmt{
              *database,
              "INSERT INTO custom_name (uuid, postfix) VALUES (?, ?) ON "
              "CONFLICT(uuid) DO UPDATE SET postfix = excluded.postfix"};
          static SQLite::Statement nametag_stmt{
              *database,
              "INSERT INTO custom_name (uuid, nametag) VALUES (?, ?) ON "
              "CONFLICT(uuid) DO UPDATE SET nametag = excluded.nametag"};
          auto &stmt = key == Key::Prefix ? prefix_stmt : key == Key::Postfix ? postfix_stmt : nametag_stmt;
          BOOST_SCOPE_EXIT_ALL(&) {
            stmt.reset();
            stmt.clearBindings();
          };
          stmt.bindNoCopy(1, it->uuid, sizeof it->uuid);
          stmt.bindNoCopy(2, str);
          stmt.exec();
          count++;

        } catch (SQLite::Exception const &ex) {
          output.error(ex.getErrorStr());
          return;
        }
      }
    }
    transaction.commit();
    output.success("commands.custom-name.success.set", {count});
  }

  static void setup(CommandRegistry *registry) {
    using namespace commands;
    commands::addEnum<Action>(registry, "custom-name-set", {{"set", Action::Set}});
    commands::addEnum<Key>(
        registry, "custom-name-key", {{"prefix", Key::Prefix}, {"postfix", Key::Postfix}});
    registry->registerOverload<SetCustomNameCommand>(
        "custom-name", mandatory<CommandParameterDataType::ENUM>(&SetCustomNameCommand::_sig, "set", "custom-name-set"),
        mandatory<CommandParameterDataType::ENUM>(&SetCustomNameCommand::key, "key", "custom-name-key"),
        mandatory(&SetCustomNameCommand::selector, "target"), mandatory(&SetCustomNameCommand::str, "str"));
  }
};

class ClearCustomNameCommand : public Command {
public:
  Action _sig;
  CommandSelector<Player> selector;
  ClearCustomNameCommand() { selector.setIncludeDeadPlayers(true); }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    int count = 0;
    SQLite::Transaction transaction(*database);
    auto &playerdb = Mod::PlayerDatabase::GetInstance().GetData();
    for (auto player : selector.results(origin)) {
      auto it = playerdb.find(player);
      static SQLite::Statement stmt{*database, "DELETE FROM custom_name WHERE uuid = ?"};
      BOOST_SCOPE_EXIT_ALL(&) {
        stmt.reset();
        stmt.clearBindings();
      };
      stmt.bindNoCopy(1, it->uuid, sizeof it->uuid);
      stmt.exec();
      count++;
    }
    transaction.commit();
    output.success("commands.custom-name.success.clear", {count});
  }

  static void setup(CommandRegistry *registry) {
    using namespace commands;
    auto ssig = commands::addEnum<Action>(registry, "custom-name-clear", {{"clear", Action::Clear}});
    registry->registerOverload<ClearCustomNameCommand>(
        "custom-name", mandatory<CommandParameterDataType::ENUM>(&ClearCustomNameCommand::_sig, "clear", ssig),
        mandatory(&ClearCustomNameCommand::selector, "target"));
  }
};

void registerCustomName(CommandRegistry *registry) {
  registry->registerCommand(
      "custom-name", "commands.custom-name.description", CommandPermissionLevel::GameMasters, CommandFlagCheat,
      CommandFlagNone);
  SetCustomNameCommand::setup(registry);
  ClearCustomNameCommand::setup(registry);
  Mod::Chat::GetInstance().AddListener(SIG("chat"), {Mod::RecursiveEventHandlerAdaptor(ChatHandler)});
}