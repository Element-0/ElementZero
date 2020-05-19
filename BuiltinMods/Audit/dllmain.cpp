#include <boost/scope_exit.hpp>

#include <base.h>
#include <Core/BinaryStream.h>
#include <Math/BlockPos.h>
#include <Net/NetworkIdentifier.h>
#include <Packet/PlayerActionPacket.h>
#include <Packet/ItemFrameDropItemPacket.h>
#include <Packet/InventoryTransactionPacket.h>

#include <dllentry.h>
#include <playerdb.h>
#include <command.h>
#include <hook.h>
#include <audit.h>

#include "settings.h"

Settings settings;
DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

namespace Mod {

void (AuditSystem::*EmitPlayerAction)(
    sigt<"action"_sig>, PlayerEntry const &, PlayerAction const &, CallbackToken<std::string> &);
void (AuditSystem::*EmitPlayerTransaction)(
    sigt<"inventory_transaction"_sig>, PlayerEntry const &, ComplexInventoryTransaction const &,
    CallbackToken<std::string> &);
void (AuditSystem::*EmitItemFrameDrop)(
    sigt<"item_frame_drop"_sig>, PlayerEntry const &, BlockPos const &, CallbackToken<std::string> &);

AuditSystem::AuditSystem() {
  EmitPlayerAction      = &AuditSystem::Emit;
  EmitPlayerTransaction = &AuditSystem::Emit;
  EmitItemFrameDrop     = &AuditSystem::Emit;
}

AuditSystem &AuditSystem::GetInstance() {
  static AuditSystem instance;
  return instance;
}

auto &mAuditSystem = AuditSystem::GetInstance();

} // namespace Mod

void PreInit() {
  if (!settings.Database.empty()) InitDatabase();
  Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), InitCommand);
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVPlayerActionPacket@@@Z",
    NetworkIdentifier const &netid, PlayerActionPacket const &pkt) {
  using namespace Mod;
  PlayerAction action;
  action.type    = pkt.type;
  action.pos     = pkt.pos;
  action.face    = pkt.face;
  action.actorId = pkt.actorId;

  auto &db = PlayerDatabase::GetInstance();
  if (auto it = db.Find(netid); it) {
    Mod::CallbackToken<std::string> token;
    (mAuditSystem.*EmitPlayerAction)(SIG("action"), *it, action, token);
    if (database && !disable_temporary) {
      static SQLite::Statement stmt{*database,
                                    "INSERT INTO audit_action "
                                    "(session, player, dimension, blocked, type, x, y, z, face) VALUES "
                                    "(?, ?, ?, ?, ?, ?, ?, ?, ?)"};
      BOOST_SCOPE_EXIT_ALL() {
        stmt.clearBindings();
        stmt.reset();
      };
      auto &session = SessionUUID();
      stmt.bindNoCopy(1, session, sizeof session);
      stmt.bindNoCopy(2, it->uuid, sizeof it->uuid);
      stmt.bind(3, it->player->getDimensionId());
      if (token)
        stmt.bindNoCopy(4, *token);
      else
        stmt.bind(4, nullptr);
      stmt.bind(5, (int) pkt.type);
      stmt.bind(6, pkt.pos.x);
      stmt.bind(7, pkt.pos.y);
      stmt.bind(8, pkt.pos.z);
      stmt.bind(9, pkt.face);
      stmt.exec();
    }
    if (!token) original(this, netid, pkt);
  }
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVInventoryTransactionPacket@@@Z",
    NetworkIdentifier const &netid, InventoryTransactionPacket const &pkt) {
  using namespace Mod;
  auto &db = PlayerDatabase::GetInstance();
  if (auto it = db.Find(netid); it) {
    Mod::CallbackToken<std::string> token;
    (mAuditSystem.*EmitPlayerTransaction)(SIG("inventory_transaction"), *it, *pkt.transaction, token);
    if (database && !disable_temporary) {
      static SQLite::Statement stmt{*database,
                                    "INSERT INTO audit_transaction "
                                    "(session, player, dimension, blocked, data) VALUES "
                                    "(?, ?, ?, ?, ?)"};
      BinaryStream outp;
      pkt.transaction->write(outp);
      auto data = outp.getAndReleaseData();
      BOOST_SCOPE_EXIT_ALL() {
        stmt.clearBindings();
        stmt.reset();
      };
      auto &session = SessionUUID();
      stmt.bindNoCopy(1, session, sizeof session);
      stmt.bindNoCopy(2, it->uuid, sizeof it->uuid);
      stmt.bind(3, it->player->getDimensionId());
      if (token)
        stmt.bindNoCopy(4, *token);
      else
        stmt.bind(4, nullptr);
      stmt.bindNoCopy(5, data);
      stmt.exec();
    }
    if (!token) original(this, netid, pkt);
  }
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVItemFrameDropItemPacket@@@Z",
    NetworkIdentifier const &netid, ItemFrameDropItemPacket const &pkt) {
  using namespace Mod;
  auto &db = PlayerDatabase::GetInstance();
  if (auto it = db.Find(netid); it) {
    Mod::CallbackToken<std::string> token;
    (mAuditSystem.*EmitItemFrameDrop)(SIG("item_frame_drop"), *it, pkt.pos, token);
    if (database && !disable_temporary) {
      static SQLite::Statement stmt{*database,
                                    "INSERT INTO audit_item_frame "
                                    "(session, player, dimension, blocked, x, y, z) VALUES "
                                    "(?, ?, ?, ?, ?, ?, ?)"};
      BOOST_SCOPE_EXIT_ALL() {
        stmt.clearBindings();
        stmt.reset();
      };
      auto &session = SessionUUID();
      stmt.bindNoCopy(1, session, sizeof session);
      stmt.bindNoCopy(2, it->uuid, sizeof it->uuid);
      stmt.bind(3, it->player->getDimensionId());
      if (token)
        stmt.bindNoCopy(4, *token);
      else
        stmt.bind(4, nullptr);
      stmt.bind(5, pkt.pos.x);
      stmt.bind(6, pkt.pos.y);
      stmt.bind(7, pkt.pos.z);
      stmt.exec();
    }
    if (!token) original(this, netid, pkt);
  }
}