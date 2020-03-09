#include "pch.h"

#include "global.h"

static bool VerifyName(std::string_view name) {
  if (name.size() > 16 || name.size() < 1) return false;
  return name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-") ==
         std::string::npos;
}

class WarpCommand : public Command {
public:
#pragma region detail
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
#pragma endregion

  void printGlobalList(CommandOrigin const &origin, CommandOutput &output) {
    static SQLite::Statement stmt_count{*world_database, "SELECT count(*) FROM global_warp"};
    BOOST_SCOPE_EXIT_ALL() { stmt_count.reset(); };
    if (stmt_count.executeStep() != true) return; // impossible actually
    auto count = stmt_count.getColumn(0).getInt();
    output.success("commands.warp.success.count.global", {count});
    if (count == 0) return;
    static SQLite::Statement stmt_list{*world_database, "SELECT name, dim, x, y, z FROM global_warp"};
    BOOST_SCOPE_EXIT_ALL() { stmt_list.reset(); };
    while (stmt_list.executeStep()) {
      auto ename = "." + stmt_list.getColumn(0).getString();
      auto dim   = stmt_list.getColumn(1).getInt();
      auto x     = (float) stmt_list.getColumn(2).getDouble();
      auto y     = (float) stmt_list.getColumn(3).getDouble();
      auto z     = (float) stmt_list.getColumn(4).getDouble();
      Vec3 pos{x, y, z};
      output.success("commands.warp.success.list", {ename, dim, pos});
    }
  }

  void printList(Mod::PlayerEntry const &ent, CommandOrigin const &origin, CommandOutput &output) {
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
  }

  void handleTo(Mod::PlayerEntry const &ent, CommandOrigin const &origin, CommandOutput &output) {
    static SQLite::Statement global_stmt{*world_database, "SELECT dim, x, y, z FROM global_warp WHERE name = ?"};
    static SQLite::Statement stmt{*world_database, "SELECT dim, x, y, z FROM warp WHERE uuid = ? AND name = ?"};
    SQLite::Statement *cstmt;
    if (name[0] == '.') {
      std::string_view vname = name;
      vname.remove_prefix(1);
      if (!VerifyName(vname)) {
        output.error("commands.warp.error.format");
        return;
      }
      cstmt = &global_stmt;
      cstmt->bindNoCopy(1, name.c_str() + 1);
    } else {
      if (!VerifyName(name)) {
        output.error("commands.warp.error.format");
        return;
      }
      cstmt = &stmt;
      cstmt->bindNoCopy(1, ent.uuid, sizeof ent.uuid);
      cstmt->bindNoCopy(2, name);
    }
    BOOST_SCOPE_EXIT_ALL(cstmt) {
      cstmt->clearBindings();
      cstmt->reset();
    };
    if (cstmt->executeStep()) {
      auto dim = cstmt->getColumn(0).getInt();
      auto x   = (float) cstmt->getColumn(1).getDouble();
      auto y   = (float) cstmt->getColumn(2).getDouble() - 1;
      auto z   = (float) cstmt->getColumn(3).getDouble();
      Vec3 pos = {x, y, z};
      ent.player->teleport(pos, {}, dim);
      output.success("commands.warp.success.warp", {name, pos});
    } else {
      output.error("commands.warp.error.warp.not.found", {name});
    }
  }

  void handleSetOrDelete(Mod::PlayerEntry const &ent, CommandOrigin const &origin, CommandOutput &output) {
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
        stmt.tryReset();
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
        stmt.tryReset();
      };
      stmt.bindNoCopy(1, ent.uuid, sizeof ent.uuid);
      stmt.bindNoCopy(2, name);
      try {
        stmt.exec();
        output.success("commands.warp.success.del", {name});
      } catch (SQLite::Exception const &ex) { output.success("commands.warp.error.del", {name, ex.what()}); }
    }
  }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      if (isList) {
        printGlobalList(origin, output);
        return;
      }
      output.error("commands.generic.error.invalidPlayer", {"/warp"});
      return;
    }
    auto ent = *Mod::PlayerDatabase::GetInstance().Find((Player *) origin.getEntity());
    if (isList) {
      printGlobalList(origin, output);
      printList(ent, origin, output);
    } else if (isTo) {
      handleTo(ent, origin, output);
    } else if (isSetOrDelete) {
      handleSetOrDelete(ent, origin, output);
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "warp", "commands.warp.description", CommandPermissionLevel::Normal, CommandFlagCheat, CommandFlagNone);
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

class GlobalWarpCommand : public Command {
  enum class SetOrDel { set, del } setOrDel;
  std::string name;

public:
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {"/global-warp"});
      return;
    }
    if (!VerifyName(name)) {
      output.error("commands.warp.error.format");
      return;
    }
    auto ent = *Mod::PlayerDatabase::GetInstance().Find((Player *) origin.getEntity());
    switch (setOrDel) {
    case SetOrDel::set: {
      static SQLite::Statement stmt{*world_database,
                                    "INSERT OR REPLACE INTO global_warp "
                                    "(name, dim, x, y, z) "
                                    "VALUES (?, ?, ?, ?, ?)"};
      BOOST_SCOPE_EXIT_ALL() {
        stmt.clearBindings();
        stmt.reset();
      };
      stmt.bindNoCopy(1, name);
      auto dim = ent.player->getDimensionId();
      auto pos = ent.player->getPos();
      stmt.bind(2, dim.value);
      stmt.bind(3, pos.x);
      stmt.bind(4, pos.y);
      stmt.bind(5, pos.z);
      stmt.exec();
      output.success("commands.global-warp.success.set", {name, pos});
    } break;
    case SetOrDel::del: {
      static SQLite::Statement stmt{*world_database, "DELETE FROM global_warp WHERE name = ?"};
      BOOST_SCOPE_EXIT_ALL() {
        stmt.clearBindings();
        stmt.reset();
      };
      stmt.bindNoCopy(1, name);
      stmt.exec();
      output.success("commands.global-warp.success.del", {name});
    } break;
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "global-warp", "commands.global-warp.description", CommandPermissionLevel::Privileged, CommandFlagCheat,
        CommandFlagNone);
    addEnum<SetOrDel>(registry, "global-warp-set-or-del", {{"set", SetOrDel::set}, {"del", SetOrDel::del}});
    registry->registerOverload<GlobalWarpCommand>(
        "global-warp",
        mandatory<CommandParameterDataType::ENUM>(&GlobalWarpCommand::setOrDel, "set-or-del", "global-warp-set-or-del"),
        mandatory(&GlobalWarpCommand::name, "name"));
  }
};

void registerWarp(CommandRegistry *registry) {
  WarpCommand::setup(registry);
  GlobalWarpCommand::setup(registry);
}