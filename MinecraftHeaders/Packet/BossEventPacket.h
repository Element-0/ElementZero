#pragma once

#include <cstdint>
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
  float percent  = 0.0f;
  unsigned color = 0, overlay = 0;
  bool darken_sky = false, fog = false;

  inline ~BossEventPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(BossEventPacket, owner) == 48);
static_assert(offsetof(BossEventPacket, name) == 72);
static_assert(offsetof(BossEventPacket, percent) == 104);
static_assert(offsetof(BossEventPacket, color) == 108);
static_assert(offsetof(BossEventPacket, overlay) == 112);
static_assert(offsetof(BossEventPacket, darken_sky) == 116);
static_assert(offsetof(BossEventPacket, fog) == 117);