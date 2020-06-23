#pragma once

#include <hook.h>

#include "NetworkIdentifier.h"
#include "../dll.h"

class Player;

class ServerNetworkHandler {
public:
  MCAPI void disconnectClient(NetworkIdentifier const &, unsigned char subid, std::string const &reason, bool hide);
  MCAPI void updateServerAnnouncement();

  void forceDisconnectClient(Player *player, bool hide) {
    CallServerClassMethod<void>("?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", this, player, hide);
  }

  BASEAPI std::string &GetMotd();
};