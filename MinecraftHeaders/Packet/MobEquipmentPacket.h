#pragma once

#include "../Core/Packet.h"
#include "../Item/ItemStack.h"
#include "../Actor/ActorRuntimeID.h"
#include "../dll.h"

class MobEquipmentPacket : public Packet {
public:
  ActorRuntimeID actorId;
  ItemStack stack;
  uint32_t inventorySlot;
  uint32_t hotbarSlot;
  bool flag192;
  char containerId; //193
  bool flag194;
  bool flag195;
  bool flag196;

  inline ~MobEquipmentPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};