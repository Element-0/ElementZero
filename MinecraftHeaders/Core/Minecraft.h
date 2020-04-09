#pragma once

#include "../dll.h"

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class Level;
class ServerNetworkHandler;
class MinecraftCommands;

class Minecraft {
public:
  MCAPI Level *getLevel() const;
  MCAPI ServerNetworkHandler *getServerNetworkHandler();
  BASEAPI MinecraftCommands *getCommands();
};