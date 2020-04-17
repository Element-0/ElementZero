#pragma once

#include <string>

#include "../Core/Packet.h"
#include "../Math/BlockPos.h"
#include "../Actor/ActorUniqueID.h"
#include "../dll.h"

enum class BossEventUpdateType {
  SHOW              = 0,
  REGISTER_PLAYER   = 1,
  UPDATE            = 1,
  HIDE              = 2,
  UNREGISTER_PLAYER = 3,
  HEALTH_PERCENT    = 4,
  TITLE             = 5,
  TOGGLE_FOG        = 6,
  TEXTURE           = 7
};

class BossEventPacket : public Packet {
public:
  int unk_val_1 = 1;
  int unk_val_2 = 2;
  ActorUniqueID owner, player;
  BossEventUpdateType type;
  std::string name;
  float percent;
  unsigned color, overlay;
  bool darken_sky, fog;

  inline ~BossEventPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(BossEventPacket, owner) == 48);
static_assert(offsetof(BossEventPacket, fog) == 117);