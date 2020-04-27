#pragma once

#include <string>


class GameVersion {
public:
  unsigned Major{}, Minor{}, Patch{}, Revision{}, Beta{};
  unsigned pad1{}, pad2{};
  std::string cache;
};

static_assert(sizeof(GameVersion) == 64);