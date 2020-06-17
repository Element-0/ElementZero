#include <cstdint>
#include <string>
#include <boost/scope_exit.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <base/playerdb.h>

#include <mods/CommandSupport.h>
#include <mods/Blacklist.h>

#include "settings.h"
#include "global.h"

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
        if (opt->xuid == 0) {
          Mod::Blacklist::GetInstance().Add(Mod::Blacklist::UUID{opt->uuid, opt->name}, reason, op);
          auto uuid = opt->uuid.asString();
          output.success("commands.ban.success.uuid", {opt->name, uuid});
        } else {
          Mod::Blacklist::GetInstance().Add(Mod::Blacklist::XUID{opt->xuid, opt->name}, reason, op);
          output.success("commands.ban.success.xuid", {opt->name, opt->xuid});
        }
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
      if (opt->xuid == 0) {
        Mod::Blacklist::GetInstance().Add(Mod::Blacklist::UUID{opt->uuid, opt->name}, reason, op);
        auto uuid = opt->uuid.asString();
        output.success("commands.ban.success.uuid", {opt->name, uuid});
      } else {
        Mod::Blacklist::GetInstance().Add(Mod::Blacklist::XUID{opt->xuid, opt->name}, reason, op);
        output.success("commands.ban.success.xuid", {opt->name, opt->xuid});
      }
    } else {
      Mod::Blacklist::GetInstance().Add(Mod::Blacklist::NAME{target}, reason, op);
      output.success("commands.ban.success.name", {target});
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerOverload<BanNameCommand>(
        "ban", mandatory(&BanNameCommand::target, "name"), optional(&BanNameCommand::reason, "reason"));
  }
};

class UnbanCommand : public Command {
  std::string target;

public:
  UnbanCommand() {}

  bool removeXUID(std::uint64_t xuid, CommandOutput &output) {
    auto &api = Mod::Blacklist::GetInstance();
    if (api.Remove(Mod::Blacklist::XUID{xuid})) {
      output.success("commands.unban.success.xuid", {xuid});
      return true;
    }
    return false;
  }
  bool removeUUID(mce::UUID const &uuid, CommandOutput &output) {
    auto &api = Mod::Blacklist::GetInstance();
    if (api.Remove(Mod::Blacklist::UUID{uuid})) {
      output.success("commands.unban.success.uuid", {uuid.asString()});
      return true;
    }
    return false;
  }
  bool removeNAME(std::string const &name, CommandOutput &output) {
    auto &api = Mod::Blacklist::GetInstance();
    if (api.Remove(Mod::Blacklist::NAME{name})) {
      output.success("commands.unban.success.name", {name});
      return true;
    }
    return false;
  }

  void removeEntry(Mod::OfflinePlayerEntry const &entry, CommandOutput &output) {
    bool flag = false;
    flag |= removeXUID(entry.xuid, output);
    flag |= removeUUID(entry.uuid, output);
    flag |= removeNAME(entry.name, output);
    if (!flag) output.error("commands.unban.error.not.found", {entry.name});
  }

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
          removeEntry(*opt, output);
        } else {
          if (!removeUUID(opt->uuid, output)) {
            output.error("commands.unban.error.not.found.uuid", {uuid.asString()});
          }
        }
        return;
      }
    }
    // XUID
    try {
      auto xuid = boost::lexical_cast<uint64_t>(target);
      if (auto opt = db.FindOffline(xuid); opt) {
        removeEntry(*opt, output);
      } else {
        if (!removeXUID(xuid, output)) { output.error("commands.unban.error.not.found.xuid", {xuid}); }
      }
      return;
    } catch (boost::bad_lexical_cast const &bad) {}
    // NAME
    if (auto opt = db.FindOffline(target); opt) {
      removeEntry(*opt, output);
    } else if (!removeNAME(target, output)) {
      output.error("commands.unban.error.not.found.name", {target});
    }

    return;
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "unban", "commands.unban.description", CommandPermissionLevel::GameMasters, CommandFlagCheat, CommandFlagNone);
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
        "force-kick", "commands.force-kick.description", CommandPermissionLevel::GameMasters, CommandFlagCheat,
        CommandFlagNone);
    registry->registerOverload<ForceKickCommand>("force-kick", mandatory(&ForceKickCommand::selector, "target"));
  }
};

void registerBanCommand(CommandRegistry *registry) {
  registry->registerCommand(
      "ban", "commands.ban.description", CommandPermissionLevel::GameMasters, CommandFlagCheat, CommandFlagNone);
  BanCommand::setup(registry);
  BanNameCommand::setup(registry);
  UnbanCommand::setup(registry);
  ForceKickCommand::setup(registry);
}