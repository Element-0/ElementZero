#include <string>
#include <boost/scope_exit.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <command.h>
#include <Command/Command.h>
#include <Command/CommandRegistry.h>
#include <Command/CommandPermissionLevel.h>
#include <Command/CommandFlag.h>
#include <Command/CommandParameterData.h>
#include <Command/CommandOutput.h>
#include <Command/CommandOrigin.h>

#include <playerdb.h>

#include "settings.h"
#include "global.h"

static void insertForUUID(char const *uuid, std::string const &name, std::string const &reason, std::string const &op) {
  static SQLite::Statement insert_uuid = {*database,
                                          "INSERT OR REPLACE INTO uuid ("
                                          "value, name, reason, source) VALUES ("
                                          "?, ?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    insert_uuid.reset();
    insert_uuid.clearBindings();
  };
  auto source = (boost::format("BANNED BY OPERATOR %s") % op).str();
  insert_uuid.bindNoCopy(1, uuid, sizeof(mce::UUID));
  insert_uuid.bindNoCopy(2, name);
  insert_uuid.bindNoCopy(3, reason);
  insert_uuid.bindNoCopy(4, source);
  insert_uuid.exec();
}

static void insertForXUID(int64_t xuid, std::string const &name, std::string const &reason, std::string const &op) {
  static SQLite::Statement insert_uuid = {*database,
                                          "INSERT OR REPLACE INTO xuid ("
                                          "value, name, reason, source) VALUES ("
                                          "?, ?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    insert_uuid.reset();
    insert_uuid.clearBindings();
  };
  auto source = (boost::format("BANNED BY COMMAND (%s)") % op).str();
  insert_uuid.bind(1, xuid);
  insert_uuid.bindNoCopy(2, name);
  insert_uuid.bindNoCopy(3, reason);
  insert_uuid.bindNoCopy(4, source);
  insert_uuid.exec();
}

static void insertForName(std::string const &name, std::string const &reason, std::string const &op) {
  static SQLite::Statement insert_uuid = {*database,
                                          "INSERT OR REPLACE INTO name ("
                                          "value, reason, source) VALUES ("
                                          "?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    insert_uuid.reset();
    insert_uuid.clearBindings();
  };
  auto source = (boost::format("BANNED BY COMMAND (%s)") % op).str();
  insert_uuid.bindNoCopy(1, name);
  insert_uuid.bindNoCopy(2, reason);
  insert_uuid.bindNoCopy(3, source);
  insert_uuid.exec();
}

template <typename E>
static void insertAuto(E const &entry, std::string const &reason, std::string const &op, CommandOutput &output) {
  if (entry.xuid == 0) {
    insertForUUID((char const *) &entry.uuid, entry.name, reason, op);
    auto uuid = entry.uuid.asString();
    output.success("commands.ban.success.uuid", {entry.name, uuid});
  } else {
    insertForXUID(entry.xuid, entry.name, reason, op);
    output.success("commands.ban.success.xuid", {entry.name, entry.xuid});
  }
}

class BanCommand : public Command {
  CommandSelector<Player> selector;
  std::string reason;

public:
  BanCommand() { selector.setIncludeDeadPlayers(true); }
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    auto op      = origin.getName();
    auto results = selector.results(origin);
    switch (results.count()) {
    case 0: output.error("commands.generic.selector.empty"); break;
    case 1: {
      auto it = *results.begin();
      if (auto opt = Mod::PlayerDatabase::GetInstance().Find(it); opt) {
        opt->netid.kick(reason);
        insertAuto(*opt, reason, op, output);
      } else {
        output.error("commands.ban.error.unexpected");
      }
    } break;
    default: output.error("commands.generic.selector.tooManyTargets"); break;
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerOverload<BanCommand>(
        "ban", mandatory(&BanCommand::selector, "target"), optional(&BanCommand::reason, "reason"));
  }
};

class BanNameCommand : public Command {
  std::string target;
  std::string reason;

public:
  BanNameCommand() {}

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    auto op = origin.getName();
    if (target.size() == 0) {
      output.error("commands.ban.error.no.empty");
      return;
    }
    auto &pdb = Mod::PlayerDatabase::GetInstance();
    if (auto opt = pdb.FindOffline(target); opt) {
      if (auto online = pdb.Find(opt->uuid); online) online->netid.kick(reason);
      insertAuto(*opt, reason, op, output);
    } else {
      insertForName(target, reason, op);
      output.success("commands.ban.success.name", {target});
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerOverload<BanNameCommand>(
        "ban", mandatory(&BanNameCommand::target, "name"), optional(&BanNameCommand::reason, "reason"));
  }
};

static bool tryRemoveUUID(mce::UUID const &uuid, CommandOutput &output) {
  static SQLite::Statement remove_uuid{*database, "DELETE FROM uuid WHERE value=?"};
  remove_uuid.bindNoCopy(1, uuid, sizeof uuid);
  BOOST_SCOPE_EXIT_ALL() {
    remove_uuid.reset();
    remove_uuid.clearBindings();
  };
  bool ret;
  if ((ret = remove_uuid.exec())) output.success("commands.unban.success.uuid", {uuid.asString()});
  return ret;
}

static bool tryRemoveXUID(int64_t xuid, CommandOutput &output) {
  static SQLite::Statement remove_xuid{*database, "DELETE FROM xuid WHERE value=?"};
  remove_xuid.bind(1, xuid);
  BOOST_SCOPE_EXIT_ALL() {
    remove_xuid.reset();
    remove_xuid.clearBindings();
  };
  bool ret;
  if ((ret = remove_xuid.exec())) output.success("commands.unban.success.xuid", {xuid});
  return ret;
}

static bool tryRemoveName(std::string const &name, CommandOutput &output) {
  static SQLite::Statement remove_name{*database, "DELETE FROM name WHERE value=?"};
  remove_name.bind(1, name);
  BOOST_SCOPE_EXIT_ALL() {
    remove_name.reset();
    remove_name.clearBindings();
  };
  bool ret;
  if ((ret = remove_name.exec())) output.success("commands.unban.success.name", {name});
  return ret;
}

static void tryRemove(Mod::OfflinePlayerEntry const &entry, CommandOutput &output) {
  auto ret = false;
  ret |= tryRemoveUUID(entry.uuid, output);
  ret |= tryRemoveXUID(entry.xuid, output);
  ret |= tryRemoveName(entry.name, output);
  if (!ret) output.error("commands.unban.error.not.found", {entry.name});
}

class UnbanCommand : public Command {
  std::string target;

public:
  UnbanCommand() {}

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (target.size() == 0) {
      output.error("commands.unban.error.no.empty");
      return;
    }
    auto &db = Mod::PlayerDatabase::GetInstance();
    // pre check for uuid
    if (target.size() == 36) {
      auto uuid = mce::UUID::fromString(target);
      if (!uuid.empty()) {
        if (auto opt = db.FindOffline(uuid); opt) {
          tryRemove(*opt, output);
        } else {
          if (!tryRemoveUUID(uuid, output)) { output.error("commands.unban.error.not.found.uuid", {uuid.asString()}); }
        }
        return;
      }
    }
    try {
      auto xuid = boost::lexical_cast<uint64_t>(target);
      if (auto opt = db.FindOffline(xuid); opt) {
        tryRemove(*opt, output);
      } else {
        if (!tryRemoveXUID(xuid, output)) { output.error("commands.unban.error.not.found.xuid", {xuid}); }
      }
      return;
    } catch (boost::bad_lexical_cast const &bad) {}
    if (auto opt = db.FindOffline(target); opt) {
      tryRemove(*opt, output);
    } else {
      if (!tryRemoveName(target, output)) { output.error("commands.unban.error.not.found.name", {target}); }
    }
    return;
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "unban", "commands.unban.description", CommandPermissionLevel::Privileged, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<UnbanCommand>("unban", mandatory(&UnbanCommand::target, "target"));
  }
};

class ForceKickCommand : public Command {
  CommandSelector<Player> selector;

public:
  ForceKickCommand() { selector.setIncludeDeadPlayers(true); }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    auto res = selector.results(origin);
    if (res.empty()) {
      output.error("commands.generic.selector.empty");
      return;
    }
    for (auto it : res) it->kick();
    output.success("commands.force-kick.success");
    return;
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "force-kick", "commands.force-kick.description", CommandPermissionLevel::Privileged, CommandFlagCheat,
        CommandFlagNone);
    registry->registerOverload<ForceKickCommand>("force-kick", mandatory(&ForceKickCommand::selector, "target"));
  }
};

void registerBanCommand(CommandRegistry *registry) {
  registry->registerCommand(
      "ban", "commands.ban.description", CommandPermissionLevel::Privileged, CommandFlagCheat, CommandFlagNone);
  BanCommand::setup(registry);
  BanNameCommand::setup(registry);
  UnbanCommand::setup(registry);
  ForceKickCommand::setup(registry);
}