#pragma once

#include "../Core/Packet.h"
#include "../Actor/ActorRuntimeID.h"
#include "../dll.h"
#include "../Math/Vec3.h"

class SpawnParticleEffectPacket : public Packet {
public:
  char dim = 0;
  ActorRuntimeID actor_id;
  Vec3 pos;
  std::string id;

  inline SpawnParticleEffectPacket() {}
  inline ~SpawnParticleEffectPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(SpawnParticleEffectPacket, dim) == 40);
static_assert(offsetof(SpawnParticleEffectPacket, id) == 72);