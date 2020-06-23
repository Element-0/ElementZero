#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "../Core/Packet.h"
#include "../Core/DataItem.h"
#include "../Math/Vec3.h"
#include "../Math/Vec2.h"
#include "../Actor/ActorDefinitionIdentifier.h"
#include "../Actor/AttributeInstanceHandle.h"
#include "../Actor/ActorLink.h"
#include "../Actor/SyncedAttribute.h"
#include "../Actor/ActorUniqueID.h"
#include "../Actor/ActorRuntimeID.h"
#include "../Actor/SynchedActorData.h"
#include "../dll.h"

class Actor;

class AddActorPacket : public Packet {
public:
  std::vector<ActorLink> links;
  ActorUniqueID uid;
  ActorRuntimeID rid;
  SynchedActorData *syncedata;
  std::vector<std::unique_ptr<DataItem>> items;
  ActorDefinitionIdentifier def_id;
  Vec3 pos, speed;
  Vec2 rot;
  float head_yaw = 0.0f;
  std::vector<AttributeInstanceHandle> attributes;
  std::vector<SyncedAttribute> synced_attribute;

  inline AddActorPacket() {}
  MCAPI AddActorPacket(Actor &);
  inline ~AddActorPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(AddActorPacket, uid) == 64);
static_assert(offsetof(AddActorPacket, rid) == 72);
static_assert(offsetof(AddActorPacket, syncedata) == 80);
static_assert(offsetof(AddActorPacket, def_id) == 112);
static_assert(offsetof(AddActorPacket, rot) == 304);
static_assert(offsetof(AddActorPacket, attributes) == 320);
static_assert(offsetof(AddActorPacket, synced_attribute) == 344);