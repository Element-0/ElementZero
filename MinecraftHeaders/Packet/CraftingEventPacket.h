#pragma once

#include <vector>

#include "../Core/Packet.h"
#include "../Core/mce.h"
#include "../Core/ContainerID.h"
#include "../Item/ItemStack.h"
#include "../dll.h"

class CraftingEventPacket : public Packet {
public:
  enum class Type { shapeless, shaped, furnace, furnace_data, multi, shulker_box };

  ContainerID container;
  Type type;
  mce::UUID uuid;
  std::vector<ItemStack> inputs, outputs;

  inline ~CraftingEventPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(CraftingEventPacket, inputs) == 64);
static_assert(offsetof(CraftingEventPacket, outputs) == 88);