#pragma once

#include "../Core/Packet.h"
#include "../Core/ContainerID.h"
#include "../dll.h"

class ContainerClosePacket : public Packet {
public:
  alignas(8) ContainerID id;

  inline ~ContainerClosePacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(ContainerClosePacket, id) == 40);