#pragma once

#include "Actor.h"
#include <hook.h>
#include "../Core/ExtendedCertificate.h"

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
    CallServerFunction<void>("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z", this, &pkt);
  }

  BASEAPI Certificate &getCertificate();
};