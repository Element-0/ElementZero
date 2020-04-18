#pragma once

#include "../Core/Packet.h"
#include "../Math/Vec3.h"
#include "../dll.h"

class SpawnExperienceOrbPacket : public Packet {
public:
  Vec3 pos;
  int amount;

  inline ~SpawnExperienceOrbPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(SpawnExperienceOrbPacket, pos) == 40);
static_assert(offsetof(SpawnExperienceOrbPacket, amount) == 52);
