#pragma once

#include <Packet/PlayerActionPacket.h>
#include <Packet/InventoryTransactionPacket.h>
#include <Actor/Player.h>

#include "event.h"
#include "playerdb.h"

#ifdef Audit_EXPORTS
#  define AUDITAPI __declspec(dllexport)
#else
#  define AUDITAPI __declspec(dllimport)
#endif

namespace Mod {

struct PlayerAction {
  PlayerActionType type;
  BlockPos pos;
  int face;
  ActorRuntimeID actorId;
};

class AuditSystem : public RecursiveEventEmitter<
                        "action"_sig, PlayerEntry const &, PlayerAction const &, CallbackToken<std::string> &>,
                    public RecursiveEventEmitter<
                        "inventory_transaction"_sig, PlayerEntry const &, ComplexInventoryTransaction const &,
                        CallbackToken<std::string> &>,
                    public RecursiveEventEmitter<
                        "item_frame_drop"_sig, PlayerEntry const &, BlockPos const &, CallbackToken<std::string> &> {
  AUDITAPI AuditSystem();

public:
  AUDITAPI static AuditSystem &GetInstance();

  USING_RECEVENTEMITTER("action", PlayerEntry const &, PlayerAction const &, CallbackToken<std::string> &);
  USING_RECEVENTEMITTER(
      "inventory_transaction", PlayerEntry const &, ComplexInventoryTransaction const &, CallbackToken<std::string> &);
  USING_RECEVENTEMITTER("item_frame_drop", PlayerEntry const &, BlockPos const &, CallbackToken<std::string> &);
};

} // namespace Mod