#include <Actor/Player.h>
#include <Actor/GameMode.h>
#include <Math/BlockPos.h>
#include <Net/NetworkIdentifier.h>
#include <Packet/PlayerActionPacket.h>
#include <Packet/InventoryTransactionPacket.h>

#include <dllentry.h>
#include <hook.h>
#include <log.h>
#include <playerdb.h>
#include <command.h>

#include "settings.h"

Settings settings;
Mode mode;

DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

void PreInit() {
  mode = settings.AllowOperator ? Mode::Permissive : Mode::Enforce;
  Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), initCommand);
}

static bool Check(Player *player, int x, int z) {
  if (mode == Mode::Disabled) return true;
  if (x < settings.Start.x || x > settings.End.x) return true;
  if (z < settings.Start.z || z > settings.End.z) return true;
  if (player->getCommandPermissionLevel() != CommandPermissionLevel::Normal && mode == Mode::Permissive) return true;
  return false;
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVPlayerActionPacket@@@Z",
    NetworkIdentifier const &netid, PlayerActionPacket const &pkt) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  if (auto it = db.Find(netid); it) {
    if (it->player->getDimensionId() == 0) {
      switch (pkt.type) {
      case PlayerActionType::START_BREAK:
      case PlayerActionType::CONTINUE_BREAK:
      case PlayerActionType::STOP_BREAK:
      case PlayerActionType::INTERACT_BLOCK:
        if (!Check(it->player, pkt.pos.x, pkt.pos.z)) return;
      default: break;
      }
    }
  }
  original(this, netid, pkt);
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVInventoryTransactionPacket@@@Z",
    NetworkIdentifier const &netid, InventoryTransactionPacket const &pkt) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  if (auto it = db.Find(netid); it) {
    switch (pkt.transaction->type) {
    case ComplexInventoryTransaction::Type::ITEM_USE: {
      auto &data    = (ItemUseInventoryTransaction const &) *pkt.transaction;
      auto composed = data.playerPos + data.clickPos;
      if (!Check(it->player, composed.x, composed.z)) {
        data.onTransactionError(*it->player, InventoryTransactionError::Unexcepted);
        return;
      }
    } break;
    case ComplexInventoryTransaction::Type::ITEM_USE_ON_ACTOR: {
      auto &data    = (ItemUseInventoryTransaction const &) *pkt.transaction;
      auto composed = data.playerPos + data.clickPos;
      if (!Check(it->player, composed.x, composed.z)) {
        data.onTransactionError(*it->player, InventoryTransactionError::Unexcepted);
        return;
      }
    } break;
    default: break;
    }
  }
  original(this, netid, pkt);
}