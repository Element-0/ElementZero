#pragma once

#include <functional>

#include <base.h>

class Player;
class Dimension;

class Level {
public:
  __declspec(dllimport) void forEachPlayer(std::function<bool(Player const &)>) const;
  __declspec(dllimport) void forEachDimension(std::function<bool(Dimension const &)>);
  BASEAPI uint64_t GetServerTick();
};