#pragma once

#include "../Core/Packet.h"
#include "../Core/NBT.h"
#include "../Math/BlockPos.h"
#include "../dll.h"

class BlockActorDataPacket : public Packet {
public:
  BlockPos pos;
  CompoundTag tag;

  inline ~BlockActorDataPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};