#pragma once

#include "Vec3.h"

class BlockPos {
public:
  int x = 0, y = 0, z = 0;
  // For ABI
  inline ~BlockPos() {}

  BlockPos operator+(BlockPos const &rhs) const noexcept { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
  constexpr bool operator==(BlockPos const &rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z; }
  constexpr bool operator!=(BlockPos const &rhs) const noexcept { return !(*this == rhs); }
  operator Vec3() { return {(float) x, (float) y, (float) z}; }
};