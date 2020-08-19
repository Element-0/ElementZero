#include "pch.h"

#include "global.h"

void (Mod::Essentials::WarpSystem::*EmitWarp)(
    sigt<"warp"_sig>, Player *, Mod::Essentials::WarpInfo const &, Mod::CallbackToken<std::string> &);
void (Mod::Essentials::WarpSystem::*EmitSetWarp)(
    sigt<"set_warp"_sig>, mce::UUID const &, Mod::Essentials::WarpInfo const &, Mod::CallbackToken<std::string> &);

#pragma region API

Mod::Essentials::WarpSystem::WarpSystem() {
  EmitWarp    = &Mod::Essentials::WarpSystem::Emit;
  EmitSetWarp = &Mod::Essentials::WarpSystem::Emit;
}

Mod::Essentials::WarpSystem &Mod::Essentials::WarpSystem::GetInstance() {
  static Mod::Essentials::WarpSystem instance;
  return instance;
}

int Mod::Essentials::WarpSystem::GetGlobalWarpCount() {
  static SQLite::Statement stmt_count{*world_database, "SELECT count(*) FROM global_warp"};
  BOOST_SCOPE_EXIT_ALL() { stmt_count.reset(); };
  if (!stmt_count.executeStep()) throw;
  return stmt_count.getColumn(0).getInt();
}

std::vector<Mod::Essentials::WarpInfo> Mod::Essentials::WarpSystem::GetGlobalWarpList() {
  static SQLite::Statement stmt_list{*world_database, "SELECT name, dim, x, y, z FROM global_warp"};
  BOOST_SCOPE_EXIT_ALL() { stmt_list.reset(); };
  std::vector<Mod::Essentials::WarpInfo> ret;

  while (stmt_list.executeStep()) {
    auto name = stmt_list.getColumn(0).getString();
    auto dim  = stmt_list.getColumn(1).getInt();
    auto x    = (float) stmt_list.getColumn(2).getDouble();
    auto y    = (float) stmt_list.getColumn(3).getDouble();
    auto z    = (float) stmt_list.getColumn(4).getDouble();
    Vec3 pos{x, y, z};
    ret.push_back({name, true, dim, pos});
  }
  return ret;
}

std::optional<Mod::Essentials::WarpInfo> Mod::Essentials::WarpSystem::GetGlobalWarp(std::string const &name) {
  static SQLite::Statement stmt{*world_database, "SELECT dim, x, y, z FROM global_warp WHERE name = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.reset();
  };
  stmt.bindNoCopy(1, name);
  if (stmt.executeStep()) {
    auto dim = stmt.getColumn(0).getInt();
    auto x   = (float) stmt.getColumn(1).getDouble();
    auto y   = (float) stmt.getColumn(2).getDouble() - 1;
    auto z   = (float) stmt.getColumn(3).getDouble();
    Vec3 pos = {x, y, z};
    return {{name, true, dim, pos}};
  }
  return {};
}

void Mod::Essentials::WarpSystem::SetGlobalWarp(Mod::Essentials::WarpInfo info) {
  static SQLite::Statement stmt{
      *world_database,
      "INSERT OR REPLACE INTO global_warp "
      "(name, dim, x, y, z) "
      "VALUES (?, ?, ?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.reset();
  };
  stmt.bindNoCopy(1, info.Name);
  auto dim = info.Dimension;
  auto pos = info.Position;
  stmt.bind(2, dim);
  stmt.bind(3, pos.x);
  stmt.bind(4, pos.y);
  stmt.bind(5, pos.z);
  stmt.exec();
}

void Mod::Essentials::WarpSystem::DelGlobalWarp(std::string const &name) {
  static SQLite::Statement stmt{*world_database, "DELETE FROM global_warp WHERE name = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.reset();
  };
  stmt.bindNoCopy(1, name);
  stmt.exec();
}

int Mod::Essentials::WarpSystem::GetWarpCount(mce::UUID const &uuid) {
  static SQLite::Statement stmt_count{*world_database, "SELECT count(*) FROM warp WHERE uuid = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt_count.clearBindings();
    stmt_count.reset();
  };
  stmt_count.bindNoCopy(1, uuid, sizeof uuid);
  if (!stmt_count.executeStep()) throw;
  return stmt_count.getColumn(0).getInt();
}

std::vector<Mod::Essentials::WarpInfo> Mod::Essentials::WarpSystem::GetWarpList(mce::UUID const &uuid) {
  static SQLite::Statement stmt_list{*world_database, "SELECT name, dim, x, y, z FROM warp WHERE uuid = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt_list.clearBindings();
    stmt_list.reset();
  };
  stmt_list.bindNoCopy(1, uuid, sizeof uuid);
  std::vector<Mod::Essentials::WarpInfo> ret;
  while (stmt_list.executeStep()) {
    auto name = stmt_list.getColumn(0).getString();
    auto dim  = stmt_list.getColumn(1).getInt();
    auto x    = (float) stmt_list.getColumn(2).getDouble();
    auto y    = (float) stmt_list.getColumn(3).getDouble();
    auto z    = (float) stmt_list.getColumn(4).getDouble();
    Vec3 pos{x, y, z};
    ret.push_back({name, false, dim, pos});
  }
  return ret;
}

std::optional<Mod::Essentials::WarpInfo>
Mod::Essentials::WarpSystem::GetWarp(mce::UUID const &uuid, std::string const &name) {
  static SQLite::Statement stmt{*world_database, "SELECT dim, x, y, z FROM warp WHERE uuid = ? AND name = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.reset();
  };
  stmt.bindNoCopy(1, uuid, sizeof uuid);
  stmt.bindNoCopy(2, name);
  if (stmt.executeStep()) {
    auto dim = stmt.getColumn(0).getInt();
    auto x   = (float) stmt.getColumn(1).getDouble();
    auto y   = (float) stmt.getColumn(2).getDouble() - 1;
    auto z   = (float) stmt.getColumn(3).getDouble();
    Vec3 pos = {x, y, z};
    return {{name, true, dim, pos}};
  }
  return {};
}

std::optional<std::string> Mod::Essentials::WarpSystem::SetWarp(mce::UUID const &uuid, Mod::Essentials::WarpInfo info) {
  SQLite::Transaction trans{*world_database};
  static SQLite::Statement stmt{
      *world_database,
      "INSERT OR REPLACE INTO warp "
      "(uuid, name, dim, x, y, z) "
      "VALUES (?, ?, ?, ?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.tryReset();
  };
  stmt.bindNoCopy(1, uuid, sizeof uuid);
  stmt.bindNoCopy(2, info.Name);
  auto dim = info.Dimension;
  auto pos = info.Position;
  stmt.bind(3, dim);
  stmt.bind(4, pos.x);
  stmt.bind(5, pos.y);
  stmt.bind(6, pos.z);
  stmt.exec();
  Mod::CallbackToken<std::string> token;
  (this->*EmitSetWarp)(SIG("set_warp"), uuid, info, token);
  if (token) return token;
  trans.commit();
  return {};
}

void Mod::Essentials::WarpSystem::DelWarp(mce::UUID const &uuid, std::string const &name) {
  static SQLite::Statement stmt{
      *world_database,
      "DELETE FROM warp "
      "WHERE uuid = ? AND name = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.tryReset();
  };
  stmt.bindNoCopy(1, uuid, sizeof uuid);
  stmt.bindNoCopy(2, name);
  stmt.exec();
}

std::optional<std::string> Mod::Essentials::WarpSystem::Warp(Player *player, Mod::Essentials::WarpInfo const &info) {
  Mod::CallbackToken<std::string> token;
  (this->*EmitWarp)(SIG("warp"), player, info, token);
  if (token) return token;
  player->teleport(info.Position, {}, info.Dimension);
  return {};
}

static bool VerifyName(std::string_view name) {
  if (name.size() > 16 || name.size() < 1) return false;
  return name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-") ==
         std::string::npos;
}

#pragma endregion

static auto &sys = Mod::Essentials::WarpSystem::GetInstance();

class WarpCommand : public Command {
public:
#pragma region detail
  enum class Action { Ui, To, List, Set, Del } action{};

  std::string name;
#pragma endregion

  void printGlobalList(CommandOrigin const &origin, CommandOutput &output) {
    auto count = sys.GetGlobalWarpCount();
    output.success("commands.warp.success.count.global", {count});
    if (count == 0) return;
    auto list = sys.GetGlobalWarpList();
    for (auto item : list)
      output.success("commands.warp.success.list", {"." + item.Name, item.Dimension, item.Position});
  }

  void printList(Mod::PlayerEntry const &ent, CommandOrigin const &origin, CommandOutput &output) {
    auto count = sys.GetWarpCount(ent.uuid);
    output.success("commands.warp.success.count", {count});
    if (count == 0) return;
    auto list = sys.GetWarpList(ent.uuid);
    for (auto item : list) output.success("commands.warp.success.list", {item.Name, item.Dimension, item.Position});
  }

  void handleTo(Mod::PlayerEntry const &ent, CommandOrigin const &origin, CommandOutput &output) {
    std::optional<Mod::Essentials::WarpInfo> opt;
    if (name[0] == '.') {
      std::string vname{name.c_str() + 1};
      if (!VerifyName(vname)) {
        output.error("commands.warp.error.format");
        return;
      }
      opt = sys.GetGlobalWarp(vname);
    } else {
      if (!VerifyName(name)) {
        output.error("commands.warp.error.format");
        return;
      }
      opt = sys.GetWarp(ent.uuid, name);
    }
    if (opt) {
      auto err = sys.Warp(ent.player, *opt);
      if (err) {
        output.error("commands.warp.error.disallow", {name, *err});
      } else {
        output.success("commands.warp.success.warp", {name, opt->Position});
      }
    } else {
      output.error("commands.warp.error.warp.not.found", {name});
    }
  }

  void handleSetOrDelete(Mod::PlayerEntry const &ent, CommandOrigin const &origin, CommandOutput &output) {
    if (!VerifyName(name)) {
      output.error("commands.warp.error.format");
      return;
    }
    switch (action) {
    case Action::Set: {
      auto dim = ent.player->getDimensionId().value;
      auto pos = ent.player->getPos();
      auto err = sys.SetWarp(ent.uuid, {name, false, dim, pos});
      if (err) {
        output.error("commands.warp.error.disallow", {name, *err});
      } else {
        output.success("commands.warp.success.set", {name, pos});
      }
    } break;
    case Action::Del: {
      sys.DelWarp(ent.uuid, name);
      output.success("commands.warp.success.del", {name});
    } break;
    default: break;
    }
  }

  static void showUi(Mod::PlayerEntry const &ent, bool local = false) {
    auto &form = Mod::FormUI::GetInstance();
    if (local) {
      auto list = sys.GetWarpList(ent.uuid);
      Json::Value root{Json::objectValue};
      root["type"]    = "form";
      root["title"]   = "ui.warp.title";
      root["content"] = "ui.warp.content.local";
      Json::Value contents{Json::arrayValue};
      auto idx = 0;
      for (auto &item : list) {
        Json::Value desc{Json::objectValue};
        desc["text"]    = item.Name.c_str();
        contents[idx++] = desc;
      }
      root["buttons"] = contents;
      form.SendModalForm(ent, root, [=](Json::Value const &json) {
        Json::FastWriter writer;
        if (json.isIntegral()) {
          auto ret = json.asUInt(0);
          if (ret < list.size()) sys.Warp(ent.player, list[ret]);
        }
      });
    } else {
      auto list = sys.GetGlobalWarpList();
      if (list.size() == 0) {
        if (sys.GetWarpCount(ent.uuid) == 0) {
          Json::Value root{Json::objectValue};
          root["type"]    = "modal";
          root["title"]   = "ui.warp.title";
          root["content"] = "ui.warp.content.empty.both";
          root["button1"] = "ui.warp.modal.button1";
          root["button2"] = "ui.warp.modal.button2";
          form.SendModalForm(ent, root, [](auto) {});
        } else {
          showUi(ent, true);
        }
        return;
      }
      Json::Value root{Json::objectValue};
      root["type"]    = "form";
      root["title"]   = "ui.warp.title";
      root["content"] = "ui.warp.content";
      Json::Value contents{Json::arrayValue};
      auto idx = 0;
      {
        Json::Value jump{Json::objectValue};
        jump["text"]    = "ui.warp.jump";
        contents[idx++] = jump;
      }
      for (auto &item : list) {
        Json::Value desc{Json::objectValue};
        desc["text"]    = item.Name.c_str();
        contents[idx++] = desc;
      }
      root["buttons"] = contents;
      form.SendModalForm(ent, root, [=](Json::Value const &json) {
        if (json.isIntegral()) {
          auto ret = json.asUInt(0);
          if (ret == 0)
            showUi(ent, true);
          else if (ret <= list.size())
            sys.Warp(ent.player, list[ret - 1]);
        }
      });
    }
  }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    auto pent = Mod::PlayerDatabase::GetInstance().Find((Player *) origin.getEntity());
    if (!pent) {
      if (action == Action::List) {
        printGlobalList(origin, output);
        return;
      }
      output.error("commands.generic.error.invalidPlayer", {"/global-warp"});
      return;
    }
    auto ent = *pent;
    switch (action) {
    case Action::Ui:
      showUi(ent);
      output.success();
      break;
    case Action::To: handleTo(ent, origin, output); break;
    case Action::List:
      printGlobalList(origin, output);
      printList(ent, origin, output);
      break;
    case Action::Set:
    case Action::Del: handleSetOrDelete(ent, origin, output); break;
    default: break;
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "warp", "commands.warp.description", CommandPermissionLevel::Any, CommandFlagCheat, CommandFlagNone);
    addEnum<Action>(registry, "warp-to", {{"to", Action::To}});
    addEnum<Action>(registry, "warp-list", {{"list", Action::List}});
    addEnum<Action>(registry, "warp-set", {{"set", Action::Set}});
    addEnum<Action>(registry, "warp-del", {{"del", Action::Del}});
    registry->registerOverload<WarpCommand>("warp"); // So the action will be default 0 (Ui)
    registry->registerOverload<WarpCommand>(
        "warp", mandatory<CommandParameterDataType::ENUM>(&WarpCommand::action, "to", "warp-to"),
        mandatory(&WarpCommand::name, "name"));
    registry->registerOverload<WarpCommand>(
        "warp", mandatory<CommandParameterDataType::ENUM>(&WarpCommand::action, "list", "warp-list"));
    registry->registerOverload<WarpCommand>(
        "warp", mandatory<CommandParameterDataType::ENUM>(&WarpCommand::action, "set", "warp-set"),
        mandatory(&WarpCommand::name, "name"));
    registry->registerOverload<WarpCommand>(
        "warp", mandatory<CommandParameterDataType::ENUM>(&WarpCommand::action, "del", "warp-del"),
        mandatory(&WarpCommand::name, "name"));
  }
};

class GlobalWarpCommand : public Command {
  enum class SetOrDel { set, del } setOrDel;
  std::string name;

public:
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    auto pent = Mod::PlayerDatabase::GetInstance().Find((Player *) origin.getEntity());
    if (!pent) {
      output.error("commands.generic.error.invalidPlayer", {"/global-warp"});
      return;
    }
    if (!VerifyName(name)) {
      output.error("commands.warp.error.format");
      return;
    }
    auto ent = *pent;
    switch (setOrDel) {
    case SetOrDel::set: {
      auto dim = ent.player->getDimensionId().value;
      auto pos = ent.player->getPos();
      sys.SetGlobalWarp({name, true, dim, pos});
      output.success("commands.warp.success.set", {name, pos});
    } break;
    case SetOrDel::del: {
      sys.DelGlobalWarp(name);
      output.success("commands.warp.success.del", {name});
    } break;
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "global-warp", "commands.global-warp.description", CommandPermissionLevel::GameMasters, CommandFlagCheat,
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
