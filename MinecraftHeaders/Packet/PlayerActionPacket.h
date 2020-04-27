#pragma once

#include "../Core/Packet.h"
#include "../Math/BlockPos.h"
#include "../Actor/ActorRuntimeID.h"
#include "../dll.h"

enum class PlayerActionType : unsigned {
  START_BREAK              = 0,
  ABORT_BREAK              = 1,
  STOP_BREAK               = 2,
  GET_UPDATED_BLOCK        = 3,
  DROP_ITEM                = 4,
  START_SLEEPING           = 5,
  STOP_SLEEPING            = 6,
  RESPAWN                  = 7,
  JUMP                     = 8,
  START_SPRINT             = 9,
  STOP_SPRINT              = 10,
  START_SNEAK              = 11,
  STOP_SNEAK               = 12,
  DIMENSION_CHANGE_REQUEST = 13,
  DIMENSION_CHANGE_ACK     = 14,
  START_GLIDE              = 15,
  STOP_GLIDE               = 16,
  BUILD_DENIED             = 17,
  CONTINUE_BREAK           = 18,
  UNK19                    = 19,
  SET_ENCHANTMENT_SEED     = 20,
  START_SWIMMING           = 21,
  STOP_SWIMMING            = 22,
  START_SPIN_ATTACK        = 23,
  STOP_SPIN_ATTACK         = 24,
  INTERACT_BLOCK           = 25,
};

class PlayerActionPacket : public Packet {
public:
  BlockPos pos;                                 // 40
  int face              = 0;                    // 52
  PlayerActionType type = (PlayerActionType) 0; // 56
  ActorRuntimeID actorId;                       // 64

  inline ~PlayerActionPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(PlayerActionPacket, actorId) == 64);
static_assert(offsetof(PlayerActionPacket, face) == 52);