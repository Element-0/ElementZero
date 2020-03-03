#pragma once

#include "Actor.h"
#include <hook.h>
#include "../Core/ExtendedCertificate.h"
#include "../Math/Vec3.h"
#include "../Math/BlockPos.h"
#include "../Core/AutomaticID.h"
#include "../Command/CommandPermissionLevel.h"

class Packet;
class ServerPlayer;

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class Player : public Actor {
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

  BASEAPI Certificate &getCertificate();
  BASEAPI BlockPos const &getSpawnPosition() const;

  __declspec(dllimport) bool canUseOperatorBlocks() const;
};