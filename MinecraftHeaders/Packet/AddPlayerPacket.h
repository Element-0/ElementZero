#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "../Core/Packet.h"
#include "../Core/DataItem.h"
#include "../Core/mce.h"
#include "../Math/Vec3.h"
#include "../Math/Vec2.h"
#include "../Item/ItemStack.h"
#include "../Level/Abilities.h"
#include "../Actor/ActorLink.h"
#include "../Actor/ActorUniqueID.h"
#include "../Actor/ActorRuntimeID.h"
#include "../Actor/SynchedActorData.h"
#include "../dll.h"

class AddPlayerPacket : public Packet {
public:
  std::vector<ActorLink> links;
  std::string username;
  mce::UUID uuid;
  ActorUniqueID uid;
  ActorRuntimeID rid;
  std::string platform_online_id;
  Vec3 pos, speed;
  Vec2 rot;
  float head_yaw = 0.0f;
  ItemStack selected_item;
  std::vector<std::unique_ptr<DataItem>> data_items;
  Abilities abilities;
  std::string device_id;
  int build_platform          = 0;
  SynchedActorData *syncedata = nullptr;

  inline ~AddPlayerPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};