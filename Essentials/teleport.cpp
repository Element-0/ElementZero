#include "pch.h"

#include "global.h"

struct TeleportEntry {
  Player *source;
  Player *target;
};

using TeleportEntryContainer = boost::multi_index_container<
    TeleportEntry,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<boost::multi_index::member<TeleportEntry, Player *, &TeleportEntry::source>>,
        boost::multi_index::ordered_unique<
            boost::multi_index::member<TeleportEntry, Player *, &TeleportEntry::target>>>>;

static TeleportEntryContainer container;

class TeleportRequest : public Command {
public:
  CommandSelector<Player> selector;

  TeleportRequest() { selector.setResultCount(1); }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {{"/tpa"}});
      return;
    }
    auto res = selector.results(origin);
    Player *target;
    switch (res.count()) {
    case 0: output.error("commands.generic.selector.empty"); return;
    case 1: target = *res.begin(); break;
    default: output.error("commands.generic.selector.tooManyTargets"); return;
    }
    auto source     = (Player *) origin.getEntity();
    auto [_, emres] = container.emplace(TeleportEntry{source, target});
    if (!emres) {
      output.error("commands.tpa.error.pending");
      return;
    }
    auto pkt = TextPacket::createTranslatedMessageWithParams(
        "commands.tpa.message.sent", {ExtendedCertificate::getIdentityName(source->getCertificate())});
    target->sendNetworkPacket(pkt);
    output.success("commands.tpa.success", {target});
  }

  static void setup(CommandRegistry *registry) {
    registry->registerCommand(
        "tpa", "commands.tpa.description", CommandPermissionLevel::Normal, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<TeleportRequest>("tpa", commands::mandatory(&TeleportRequest::selector, "selector"));
  }
};

class TeleportAccept : public Command {
public:
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {{"/tpaccept"}});
      return;
    }
    auto it = container.get<1>().find((Player *) origin.getEntity());
    if (it == container.get<1>().end()) {
      output.error("commands.tpa.error.empty");
      return;
    }
    auto pos = it->target->getPos();
    auto dim = it->target->getDimensionId();
    pos.y -= 1;
    auto source = it->source;
    source->teleportTo(pos, false, dim.value, 0);
    container.get<1>().erase(it);
    output.success("commands.tpaccept.success", {source});
  }

  static void setup(CommandRegistry *registry) {
    registry->registerCommand(
        "tpaccept", "commands.tpaccept.description", CommandPermissionLevel::Normal, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<TeleportAccept>("tpaccept");
  }
};
class TeleportDeny : public Command {
public:
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    DEF_LOGGER("DEBUG");
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {{"/tpdeny"}});
      return;
    }
    auto it = container.get<1>().find((Player *) origin.getEntity());
    if (it == container.get<1>().end()) {
      output.error("commands.tpa.error.empty");
      return;
    }
    auto pkt = TextPacket::createTextPacket<TextPacketType::SystemMessage>("commands.tpa.message.reject");
    it->source->sendNetworkPacket(pkt);
    auto source = it->source;
    container.get<1>().erase(it);
    output.success("commands.tpdeny.success", {source});
  }

  static void setup(CommandRegistry *registry) {
    registry->registerCommand(
        "tpdeny", "commands.tpdeny.description", CommandPermissionLevel::Normal, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<TeleportDeny>("tpdeny");
  }
};

static void onPlayerLeft(Mod::PlayerEntry const &entry) {
  container.get<0>().erase(entry.player);
  container.get<1>().erase(entry.player);
}

void registerTeleport(CommandRegistry *registry) {
  Mod::PlayerDatabase::GetInstance().AddListener(SIG("left"), onPlayerLeft);
  TeleportRequest::setup(registry);
  TeleportAccept::setup(registry);
  TeleportDeny::setup(registry);
}