#pragma once

#include "Player.h"
#include "../dll.h"

class GameMode {
public:
  Player *player;

  MCAPI virtual ~GameMode();
};