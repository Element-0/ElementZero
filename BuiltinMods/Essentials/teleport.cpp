#include "pch.h"

#include "global.h"

using namespace Mod::Scheduler;

struct TeleportEntry {
  Player *source;
  Player *target;
  Token token;
};

using TeleportEntryContainer = boost::multi_index_container<
    TeleportEntry,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<boost::multi_index::member<TeleportEntry, Player *, &TeleportEntry::source>>,
        boost::multi_index::ordered_unique<boost::multi_index::member<TeleportEntry, Player *, &TeleportEntry::target>>,
        boost::multi_index::ordered_unique<boost::multi_index::member<TeleportEntry, Token, &TeleportEntry::token>>>>;

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
    auto source = (Player *) origin.getEntity();
    bool failed = false;
    if (container.get<0>().count(source)) {
      output.error("commands.tpa.error.pending.source");
      failed = true;
    }
    if (container.get<1>().count(target)) {
      output.error("commands.tpa.error.pending.target");
      failed = true;
    }
    if (failed) return;
    auto token = SetTimeOut(std::chrono::seconds(settings.commands.teleport.timeout), [](Token token) {
      auto &db = container.get<2>();
      if (auto it = db.find(token); it != db.end()) {
        auto pkt = TextPacket::createTranslatedMessageWithParams("commands.tpa.message.timeout");
        it->target->sendNetworkPacket(pkt);
        db.erase(it);
      }
    });
    container.emplace(TeleportEntry{source, target, token});
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
    source->teleport(pos, {0}, dim);
    ClearTimeOut(it->token);
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
    ClearTimeOut(it->token);
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
  auto &left = container.get<0>();
  if (auto it = left.find(entry.player); it != left.end()) {
    ClearTimeOut(it->token);
    left.erase(it);
  }
  auto &right = container.get<1>();
  if (auto it = right.find(entry.player); it != right.end()) {
    ClearTimeOut(it->token);
    right.erase(it);
  }
}

void registerTeleport(CommandRegistry *registry) {
  Mod::PlayerDatabase::GetInstance().AddListener(SIG("left"), onPlayerLeft);
  TeleportRequest::setup(registry);
  TeleportAccept::setup(registry);
  TeleportDeny::setup(registry);
}