#include <boost/scope_exit.hpp>

#include <Actor/Player.h>
#include <Math/BlockPos.h>
#include <Net/NetworkIdentifier.h>
#include <Packet/PlayerActionPacket.h>
#include <Packet/InventoryTransactionPacket.h>

#include <dllentry.h>
#include <playerdb.h>
#include <hook.h>
#include <base.h>
#include <audit.h>

#include "settings.h"
#include "serialization.h"

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

AuditSystem::AuditSystem() {
  EmitPlayerAction      = &AuditSystem::Emit;
  EmitPlayerTransaction = &AuditSystem::Emit;
}

AuditSystem &AuditSystem::GetInstance() {
  static AuditSystem instance;
  return instance;
}

auto &mAuditSystem = AuditSystem::GetInstance();

} // namespace Mod

void PreInit() {
  if (!settings.Database.empty()) InitDatabase();
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
    if (database) {
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
    if (database) {
      static SQLite::Statement stmt{*database,
                                    "INSERT INTO audit_transaction "
                                    "(session, player, dimension, blocked, data) VALUES "
                                    "(?, ?, ?, ?, ?)"};
      auto data = SerializeInventoryTransaction(*pkt.transaction);
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