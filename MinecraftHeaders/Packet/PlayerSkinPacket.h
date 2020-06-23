#pragma once

#include "../Core/mce.h"
#include "../Core/Packet.h"
#include "../Actor/Skin/SerializedSkin.h"
#include "../dll.h"

class PlayerSkinPacket : public Packet {
public:
  mce::UUID uuid;
  SerializedSkin skin;
  std::string new_name, old_name;

  inline ~PlayerSkinPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(PlayerSkinPacket, new_name) == 0x200);