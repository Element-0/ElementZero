#pragma once

#include <vector>

#include "../Core/Packet.h"
#include "../Core/mce.h"
#include "../Actor/ActorUniqueID.h"
#include "../Actor/Skin/SerializedSkin.h"
#include "../dll.h"

class PlayerListPacket : public Packet {
public:
  std::vector<class PlayerListEntry> entries;
  enum PlayerListPacketType type;

  inline ~PlayerListPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

class PlayerListEntry {
public:
  ActorUniqueID uid;
  mce::UUID uuid;
  std::string name, xuid, platform_online_id;
  enum BuildPlatform platform;
  SerializedSkin skin;
  bool teacher, host;

  MCAPI ~PlayerListEntry();
  MCAPI void write(BinaryStream &) const;
  MCAPI PacketReadResult read(ReadOnlyBinaryStream &);
};

enum PlayerListPacketType { Add, Remove };