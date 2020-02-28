#include <dllentry.h>
#include <playerdb.h>
#include <hook.h>
#include <log.h>
#include <Net/NetworkIdentifier.h>
#include <Packet/MobEquipmentPacket.h>
#include <Packet/ActorFallPacket.h>
#include <Packet/TextPacket.h>
#include <Item/ItemStack.h>
#include <Item/Item.h>

DEF_LOGGER("BAC");

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
      LOGI("\"%s\"(%d) has been detected using: structure block exploit") % it->name;
    }
  }
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
    NetworkIdentifier *netid, void *packet) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  if (auto it = db.Find(*netid); it) {
    if (it->player->canUseOperatorBlocks()) {
      original(this, netid, packet);
    } else {
      LOGI("\"%s\"(%d) has been detected using: command block exploit") % it->name;
    }
  }
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVMobEquipmentPacket@@@Z",
    NetworkIdentifier *netid, MobEquipmentPacket *packet) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  auto it  = db.Find(*netid);
  if (!it) return;
  // is offhand container
  if (packet->containerId == 119) {
    auto &stack = packet->stack;
    // detect if it can be offhand
    if (auto item = stack.getItem(); item) {
      if (!item->getAllowOffhand() || stack.getStackSize() > stack.getMaxStackSize()) {
        LOGI("\"%s\"(%d) has been detected using: offhand") % it->name % it->xuid;
        return;
      }
    }
  }
  original(this, netid, packet);
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVActorFallPacket@@@Z", NetworkIdentifier *netid,
    ActorFallPacket *packet) {
  if (packet->fallDistance < 0.1) {
    auto &db = Mod::PlayerDatabase::GetInstance();
    auto it  = db.Find(*netid);
    if (!it) return;
    LOGI("\"%s\"(%d) has been detected using: No fall") % it->name % it->xuid;
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
    return;
  }
  if (packet->source != it->name) {
    LOGI("\"%s\"(%d) has been detected using: fake name") % it->name % it->xuid;
    return;
  }
  original(this, netid, packet);
}