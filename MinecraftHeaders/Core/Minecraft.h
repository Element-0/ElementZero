#pragma once

#include "../dll.h"

#include <modutils.h>

class Level;
class ServerNetworkHandler;
class MinecraftCommands;

class Minecraft {
public:
  MCAPI Level *getLevel() const;
  MCAPI ServerNetworkHandler *getServerNetworkHandler();
  BASEAPI MinecraftCommands *getCommands();
};