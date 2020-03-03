#include "pch.h"

#include "global.h"

static bool VerifyName(std::string const &name) {
  if (name.size() > 16 || name.size() < 1) return false;
  return name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") ==
         std::string::npos;
}

class WarpCommand : public Command {
public:
  enum class To { placeholder };
  enum class List { placeholder };
  enum class SetOrDelete { set, del };

  union {
    To to;
    List list;
    SetOrDelete setOrDelete;
  };

  bool isTo;
  bool isList;
  bool isSetOrDelete;

  std::string name;

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {"/warp"});
      return;
    }
    auto ent = *Mod::PlayerDatabase::GetInstance().Find((Player *) origin.getEntity());
    if (isList) {
      static SQLite::Statement stmt_count{*world_database, "SELECT count(*) FROM warp WHERE uuid = ?"};
      BOOST_SCOPE_EXIT_ALL() {
        stmt_count.clearBindings();
        stmt_count.reset();
      };
      stmt_count.bindNoCopy(1, ent.uuid, sizeof ent.uuid);
      if (stmt_count.executeStep() != true) return; // impossible actually
      auto count = stmt_count.getColumn(0).getInt();
      output.success("commands.warp.success.count", {count});
      if (count == 0) return;
      static SQLite::Statement stmt_list{*world_database, "SELECT name, dim, x, y, z FROM warp WHERE uuid = ?"};
      BOOST_SCOPE_EXIT_ALL() {
        stmt_list.clearBindings();
        stmt_list.reset();
      };
      stmt_list.bindNoCopy(1, ent.uuid, sizeof ent.uuid);
      while (stmt_list.executeStep()) {
        auto ename = stmt_list.getColumn(0).getString();
        auto dim   = stmt_list.getColumn(1).getInt();
        auto x     = (float) stmt_list.getColumn(2).getDouble();
        auto y     = (float) stmt_list.getColumn(3).getDouble();
        auto z     = (float) stmt_list.getColumn(4).getDouble();
        Vec3 pos{x, y, z};
        output.success("commands.warp.success.list", {ename, dim, pos});
      }
    } else if (isTo) {
      if (!VerifyName(name)) {
        output.error("commands.warp.error.format");
        return;
      }
      static SQLite::Statement stmt{*world_database, "SELECT dim, x, y, z FROM warp WHERE uuid = ? AND name = ?"};
      BOOST_SCOPE_EXIT_ALL() {
        stmt.clearBindings();
        stmt.reset();
      };
      stmt.bindNoCopy(1, ent.uuid, sizeof ent.uuid);
      stmt.bindNoCopy(2, name);
      if (stmt.executeStep()) {
        auto dim = stmt.getColumn(0).getInt();
        auto x   = (float) stmt.getColumn(1).getDouble();
        auto y   = (float) stmt.getColumn(2).getDouble() - 1;
        auto z   = (float) stmt.getColumn(3).getDouble();
        Vec3 pos = {x, y, z};
        ent.player->teleport(pos, {}, dim);
        output.success("commands.warp.success.warp", {name, pos});
      } else {
        output.error("commands.warp.error.warp.not.found", {name});
      }
    } else if (isSetOrDelete) {
      if (!VerifyName(name)) {
        output.error("commands.warp.error.format");
        return;
      }
      if (setOrDelete == SetOrDelete::set) {
        static SQLite::Statement stmt{*world_database,
                                      "INSERT OR REPLACE INTO warp "
                                      "(uuid, name, dim, x, y, z) "
                                      "VALUES (?, ?, ?, ?, ?, ?)"};
        BOOST_SCOPE_EXIT_ALL() {
          stmt.clearBindings();
          stmt.reset();
        };
        stmt.bindNoCopy(1, ent.uuid, sizeof ent.uuid);
        stmt.bindNoCopy(2, name);
        auto dim = ent.player->getDimensionId();
        auto pos = ent.player->getPos();
        stmt.bind(3, dim.value);
        stmt.bind(4, pos.x);
        stmt.bind(5, pos.y);
        stmt.bind(6, pos.z);
        try {
          stmt.exec();
          output.success("commands.warp.success.set", {name, pos});
        } catch (SQLite::Exception const &ex) { output.success("commands.warp.error.set", {name, ex.what()}); }
      } else {
        static SQLite::Statement stmt{*world_database,
                                      "DELETE FROM warp "
                                      "WHERE uuid = ? AND name = ?"};
        BOOST_SCOPE_EXIT_ALL() {
          stmt.clearBindings();
          stmt.reset();
        };
        stmt.bindNoCopy(1, ent.uuid, sizeof ent.uuid);
        stmt.bindNoCopy(2, name);
        try {
          stmt.exec();
          output.success("commands.warp.success.del", {name});
        } catch (SQLite::Exception const &ex) { output.success("commands.warp.error.del", {name, ex.what()}); }
      }
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "warp", "commands.warp.decsription", CommandPermissionLevel::Normal, CommandFlagCheat, CommandFlagNone);
    addEnum<To>(registry, "warp-to", {{"to", To::placeholder}});
    addEnum<List>(registry, "warp-list", {{"list", List::placeholder}});
    addEnum<SetOrDelete>(registry, "warp-set-or-del", {{"set", SetOrDelete::set}, {"del", SetOrDelete::del}});
    registry->registerOverload<WarpCommand>(
        "warp", mandatory<CommandParameterDataType::ENUM>(&WarpCommand::to, "to", "warp-to", &WarpCommand::isTo),
        mandatory(&WarpCommand::name, "name"));
    registry->registerOverload<WarpCommand>(
        "warp",
        mandatory<CommandParameterDataType::ENUM>(&WarpCommand::list, "list", "warp-list", &WarpCommand::isList));
    registry->registerOverload<WarpCommand>(
        "warp",
        mandatory<CommandParameterDataType::ENUM>(
            &WarpCommand::setOrDelete, "set-or-del", "warp-set-or-del", &WarpCommand::isSetOrDelete),
        mandatory(&WarpCommand::name, "name"));
  }
};

void registerWarp(CommandRegistry *registry) { WarpCommand::setup(registry); }