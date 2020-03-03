#pragma once

struct Vec3 {
  float x, y, z;
  // For ABI
  inline ~Vec3() {}

  Vec3 operator+(Vec3 const &rhs) const noexcept { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
  constexpr bool operator==(Vec3 const &rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z; }
  constexpr bool operator!=(Vec3 const &rhs) const noexcept { return !(*this == rhs); }
};