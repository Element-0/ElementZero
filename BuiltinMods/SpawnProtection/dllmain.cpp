#include <algorithm>

#include <Block/BlockSource.h>
#include <Block/Block.h>
#include <Block/BlockLegacy.h>
#include <Block/VanillaBlockTypes.h>
#include <Actor/Player.h>
#include <Math/BlockPos.h>
#include <Net/NetworkIdentifier.h>
#include <Packet/PlayerActionPacket.h>
#include <Packet/InventoryTransactionPacket.h>

#include <dllentry.h>
#include <hook.h>

#include <base/log.h>
#include <base/playerdb.h>
#include <mods/CommandSupport.h>
#include <mods/Audit.h>

#include "settings.h"

Settings settings;
Mode mode;

static void FixCoordinate() {
  auto [sx, ex]    = std::minmax({settings.Start.x, settings.End.x});
  auto [sz, ez]    = std::minmax({settings.Start.z, settings.End.z});
  settings.Start.x = sx;
  settings.End.x   = ex;
  settings.Start.z = sz;
  settings.End.z   = ez;
}

DEFAULT_SETTINGS_WITH_FIX(settings, FixCoordinate());

void dllenter() {}
void dllexit() {}

void AfterReload() {}

void checkAction(Mod::PlayerEntry const &, Mod::PlayerAction const &, Mod::CallbackToken<std::string> &);
void checkInventoryTransaction(
    Mod::PlayerEntry const &, ComplexInventoryTransaction const &, Mod::CallbackToken<std::string> &);
void checkItemFrameDrop(Mod::PlayerEntry const &, BlockPos const &, Mod::CallbackToken<std::string> &);

void PreInit() {
  mode = settings.AllowOperator ? Mode::Permissive : Mode::Enforce;
  Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), initCommand);
  Mod::AuditSystem::GetInstance().AddListener(SIG("action"), {Mod::RecursiveEventHandlerAdaptor(checkAction)});
  Mod::AuditSystem::GetInstance().AddListener(
      SIG("inventory_transaction"), {Mod::RecursiveEventHandlerAdaptor(checkInventoryTransaction)});
  Mod::AuditSystem::GetInstance().AddListener(
      SIG("item_frame_drop"), {Mod::RecursiveEventHandlerAdaptor(checkItemFrameDrop)});
}

static bool Check(Player *player, int x, int z) {
  if (mode == Mode::Disabled) return true;
  if (x < settings.Start.x || x > settings.End.x) return true;
  if (z < settings.Start.z || z > settings.End.z) return true;
  if (player->getCommandPermissionLevel() != CommandPermissionLevel::Any && mode == Mode::Permissive) return true;
  return false;
}

void checkAction(
    Mod::PlayerEntry const &entry, Mod::PlayerAction const &action, Mod::CallbackToken<std::string> &token) {
  DEF_LOGGER("debug");
  if (entry.player->getDimensionId() == 0) {
    switch (action.type) {
    case PlayerActionType::START_BREAK:
    case PlayerActionType::CONTINUE_BREAK:
    case PlayerActionType::INTERACT_BLOCK:
      if (!Check(entry.player, action.pos.x, action.pos.z)) token("Blocked by SpawnProtection");
    default: break;
    }
  }
}
void checkInventoryTransaction(
    Mod::PlayerEntry const &entry, ComplexInventoryTransaction const &transaction,
    Mod::CallbackToken<std::string> &token) {
  if (entry.player->getDimensionId() == 0) {
    switch (transaction.type) {
    case ComplexInventoryTransaction::Type::ITEM_USE: {
      auto &data = (ItemUseInventoryTransaction const &) transaction;
      switch (data.actionType) {
      case ItemUseInventoryTransaction::Type::USE_ITEM_ON:
        if (!Check(entry.player, data.pos.x, data.pos.z)) {
          auto &block  = direct_access<BlockSource *>(entry.player, 0x320)->getBlock(data.pos);
          auto &legacy = block.LegacyBlock;
          if (!legacy.isInteractiveBlock() || legacy.BlockID == VanillaBlockTypes::mItemFrame->BlockID ||
              entry.player->isSneaking()) {
            data.onTransactionError(*entry.player, InventoryTransactionError::Unexcepted);
            token("Blocked by SpawnProtection");
          }
        }
        break;
      case ItemUseInventoryTransaction::Type::USE_ITEM: break;
      case ItemUseInventoryTransaction::Type::DESTROY:
        if (!Check(entry.player, data.pos.x, data.pos.z)) {
          data.onTransactionError(*entry.player, InventoryTransactionError::Unexcepted);
          token("Blocked by SpawnProtection");
        }
        break;
      }
    } break;
    case ComplexInventoryTransaction::Type::ITEM_USE_ON_ACTOR: {
      auto &data    = (ItemUseOnActorInventoryTransaction const &) transaction;
      auto composed = data.playerPos + data.clickPos;
      if (!Check(entry.player, composed.x, composed.z)) {
        data.onTransactionError(*entry.player, InventoryTransactionError::Unexcepted);
        token("Blocked by SpawnProtection");
      }
    } break;
    default: break;
    }
  }
}

void checkItemFrameDrop(Mod::PlayerEntry const &entry, BlockPos const &pos, Mod::CallbackToken<std::string> &token) {
  if (entry.player->getDimensionId() == 0) {
    if (!Check(entry.player, pos.x, pos.z)) token("Blocked by SpawnProtection");
  }
}