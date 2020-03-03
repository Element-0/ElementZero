#pragma once

class Level;
class ServerNetworkHandler;

class Minecraft {
public:
  __declspec(dllimport) Level *getLevel() const;
  __declspec(dllimport) ServerNetworkHandler *getServerNetworkHandler();
};