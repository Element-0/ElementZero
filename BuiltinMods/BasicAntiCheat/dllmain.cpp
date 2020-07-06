#include <Net/NetworkIdentifier.h>
#include <Packet/MobEquipmentPacket.h>
#include <Packet/ActorFallPacket.h>
#include <Packet/TextPacket.h>
#include <Item/ItemStack.h>
#include <Item/Potion.h>
#include <Item/ItemDescriptor.h>
#include <Item/Item.h>

#include <dllentry.h>
#include <hook.h>

#include <base/log.h>
#include <base/playerdb.h>
#include <mods/BasicAntiCheat.h>

DEF_LOGGER("BAC");

namespace Mod {

void (AntiCheat::*EmitDetected)(sigt<"detected"_sig>, std::string_view, PlayerEntry const &);

AntiCheat::AntiCheat() { EmitDetected = &AntiCheat::Emit; }

AntiCheat &AntiCheat::GetInstance() {
  static AntiCheat instance;
  return instance;
}

auto &mAntiCheat = AntiCheat::GetInstance();

} // namespace Mod

using namespace Mod;

void dllenter() {}
void dllexit() {}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVStructureBlockUpdatePacket@@@Z",
    NetworkIdentifier *netid, void *packet) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  if (auto it = db.Find(*netid); it) {
    if (it->player->canUseOperatorBlocks()) {
      original(this, netid, packet);
    } else {
      LOGI("\"%s\"(%d) has been detected using: structure block exploit") % it->name % it->xuid;
      (mAntiCheat.*EmitDetected)(SIG("detected"), "edit_block", *it);
    }
  }
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVActorFallPacket@@@Z", NetworkIdentifier *netid,
    ActorFallPacket *packet) {
  if (packet->fallDistance < 0.1) {
    auto &db = Mod::PlayerDatabase::GetInstance();
    auto it  = db.Find(*netid);
    if (!it) return;
    LOGI("\"%s\"(%d) has been detected using: No fall") % it->name % it->xuid;
    (mAntiCheat.*EmitDetected)(SIG("detected"), "nofall", *it);
    packet->inVoid = true;
  }
  original(this, netid, packet);
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z", NetworkIdentifier *netid,
    TextPacket *packet) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  auto it  = db.Find(*netid);
  if (!it) return;
  if (packet->type != TextPacketType::Chat) {
    LOGI("\"%s\"(%d) has been detected using: chat hack") % it->name % it->xuid;
    (mAntiCheat.*EmitDetected)(SIG("detected"), "chat_hack", *it);
    return;
  }
  if (packet->source != it->name) {
    LOGI("\"%s\"(%d) has been detected using: fake name") % it->name % it->xuid;
    (mAntiCheat.*EmitDetected)(SIG("detected"), "fake_name", *it);
    return;
  }
  original(this, netid, packet);
}
