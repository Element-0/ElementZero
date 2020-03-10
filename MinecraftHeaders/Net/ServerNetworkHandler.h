#pragma once

#include "NetworkIdentifier.h"
#include <hook.h>

class Player;

class ServerNetworkHandler {
public:
  __declspec(dllimport) void disconnectClient(NetworkIdentifier const &, std::string const &reason, bool hide);
  __declspec(dllimport) void updateServerAnnouncement();

  void forceDisconnectClient(Player *player, bool hide) {
    CallServerClassMethod<void>("?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", this, player, hide);
  }

  BASEAPI std::string &GetMotd();
};