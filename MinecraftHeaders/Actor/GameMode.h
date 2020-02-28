#pragma once

#include "Player.h"

class GameMode {
public:
  Player *player;

  __declspec(dllimport) virtual ~GameMode();
};