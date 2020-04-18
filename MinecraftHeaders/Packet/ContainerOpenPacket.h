#pragma once

#include "../Core/Packet.h"
#include "../Core/ContainerID.h"
#include "../Actor/ActorUniqueID.h"
#include "../Math/BlockPos.h"
#include "../Container/ContainerType.h"
#include "../dll.h"

class ContainerOpenPacket : public Packet {
public:
  ContainerID id;
  ContainerType type;
  BlockPos pos;
  ActorUniqueID actor_id;

  inline ~ContainerOpenPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(ContainerOpenPacket, id) == 40);
static_assert(offsetof(ContainerOpenPacket, type) == 41);
static_assert(offsetof(ContainerOpenPacket, pos) == 44);
static_assert(offsetof(ContainerOpenPacket, actor_id) == 56);