#pragma once

#include "../Core/Packet.h"
#include "../Math/Vec3.h"
#include "../dll.h"

class MoveActorAbsolutePacket : public Packet {
public:
  enum struct Flags { teleport = 1, on_ground = 2 };
  Flags flags;
  Vec3 pos;
  char pitch, head_yaw, yaw;

  inline ~MoveActorAbsolutePacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};