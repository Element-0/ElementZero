#pragma once

#include "../dll.h"

class Level;
class ServerNetworkHandler;

class Minecraft {
public:
  MCAPI Level *getLevel() const;
  MCAPI ServerNetworkHandler *getServerNetworkHandler();
};