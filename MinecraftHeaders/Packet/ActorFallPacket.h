#pragma once

#include "../Core/Packet.h"
#include "../Actor/ActorRuntimeID.h"
#include "../dll.h"

class ActorFallPacket : public Packet {
public:
  ActorRuntimeID actorId;
  float fallDistance = 0.0f;
  bool inVoid        = false;

  inline ~ActorFallPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};