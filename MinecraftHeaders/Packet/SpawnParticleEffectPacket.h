#pragma once

#include "../Core/Packet.h"
#include "../Actor/ActorRuntimeID.h"
#include "../dll.h"
#include "../Math/Vec3.h"

class SpawnParticleEffectPacket : public Packet {
public:
  char dimensionId;
  ActorRuntimeID actorId;
  Vec3 pos;
  std::string id;

  inline SpawnParticleEffectPacket() {}
  inline ~SpawnParticleEffectPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};