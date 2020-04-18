#pragma once

#include <vector>

#include "../Core/Packet.h"
#include "../Core/HashedString.h"
#include "../Actor/ActorRuntimeID.h"
#include "../dll.h"

class UpdateAttributesPacket : public Packet {
public:
  struct AttributeData {
    float value, min, max, def;
    HashedString name;
  };

  ActorRuntimeID rid;
  std::vector<AttributeData> data;

  inline ~UpdateAttributesPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};