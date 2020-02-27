#pragma once

#include "../Core/Packet.h"
#include "../Item/ItemStack.h"
#include "../Actor/ActorRuntimeID.h"

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
  __declspec(dllimport) virtual MinecraftPacketIds getId() const;
  __declspec(dllimport) virtual std::string getName() const;
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual PacketReadResult read(ReadOnlyBinaryStream &);
};