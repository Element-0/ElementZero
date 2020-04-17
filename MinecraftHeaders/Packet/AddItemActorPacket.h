#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "../Core/Packet.h"
#include "../Core/DataItem.h"
#include "../Math/Vec3.h"
#include "../Actor/ActorUniqueID.h"
#include "../Actor/ActorRuntimeID.h"
#include "../Item/ItemStack.h"
#include "../dll.h"

class AddItemActorPacket : public Packet {
public:
  std::vector<std::unique_ptr<DataItem>> items;
  uint64_t type;
  ActorUniqueID uid;
  ActorRuntimeID rid;
  ItemStack stack;
  Vec3 pos, speed;
  bool from_fishing;

  inline ~AddItemActorPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(AddItemActorPacket, from_fishing) == 248);