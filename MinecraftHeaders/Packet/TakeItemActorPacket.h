#pragma once

#include "../Core/Packet.h"
#include "../Actor/ActorRuntimeID.h"
#include "../dll.h"

class TakeItemActorPacket : public Packet {
public:
  ActorRuntimeID entity, target;

  inline ~TakeItemActorPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};