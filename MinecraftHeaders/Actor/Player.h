#pragma once

#include "Actor.h"
#include <hook.h>
#include "../Core/ExtendedCertificate.h"
#include "../Math/Vec3.h"
#include "../Math/BlockPos.h"
#include "../Core/AutomaticID.h"
#include "../Command/CommandPermissionLevel.h"
#include <vector>

class Packet;
class ServerPlayer;
class NetworkIdentifier;

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class Player : public Mob {
public:
  inline ServerPlayer *asServerPlayer() const noexcept {
    return const_cast<ServerPlayer *>(reinterpret_cast<ServerPlayer const *>(this));
  }

  inline void sendNetworkPacket(Packet &pkt) const {
    CallServerClassMethod<void>("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z", this, &pkt);
  }

  void teleportTo(Vec3 const &vec, bool flag, int a, int b) const {
    CallServerClassMethod<void>("?teleportTo@Player@@UEAAXAEBVVec3@@_NHH@Z", this, &vec, flag, a, b);
  }

  inline CommandPermissionLevel getCommandPermissionLevel() const {
    return CallServerClassMethod<CommandPermissionLevel>(
        "?getCommandPermissionLevel@Player@@UEBA?AW4CommandPermissionLevel@@XZ", this);
  }

  inline float getDestroySpeed(class Block const& a0) const {
      return CallServerClassMethod<float>(
          "?getDestroySpeed@Player@@QEBAMAEBVBlock@@@Z", this, &a0);
  }

  inline bool isCreative() const {
      return CallServerClassMethod<bool>(
          "?isCreative@Player@@UEBA_NXZ", this);
  }

  inline std::vector<class ItemStack const*> getAllHand() const {
      return CallServerClassMethod<std::vector<class ItemStack const*>>(
          "?getAllHand@Player@@UEBA?AV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ", this);
  }

  inline class ItemStack const& getCarriedItem() const {
      return CallServerClassMethod<ItemStack const& >(
          "?getCarriedItem@Player@@UEBAAEBVItemStack@@XZ", this);
  }

  BASEAPI Certificate &getCertificate();
  BASEAPI NetworkIdentifier const &getNetworkIdentifier() const;
  BASEAPI BlockPos const &getSpawnPosition() const;
  BASEAPI void kick();

};