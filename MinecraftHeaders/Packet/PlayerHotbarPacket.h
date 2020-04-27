#pragma once

#include "../Core/Packet.h"
#include "../Core/ContainerID.h"
#include "../dll.h"

class PlayerHotbarPacket : public Packet {
public:
  int slot = 0;
  ContainerID container = ContainerID::Invalid;
  bool selected = false;

  inline ~PlayerHotbarPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};