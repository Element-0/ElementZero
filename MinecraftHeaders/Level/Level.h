#pragma once

#include <functional>

#include <base.h>

#include "LevelDataWrapper.h"
#include "../dll.h"

class Player;
class Dimension;

class Level {
public:
  MCAPI void forEachPlayer(std::function<bool(Player const &)>) const;
  MCAPI void forEachDimension(std::function<bool(Dimension const &)>);
  BASEAPI uint64_t GetServerTick();
  BASEAPI LevelDataWrapper &GetLevelDataWrapper();
};