#pragma once

struct Vec3 {
  float x, y, z;
  // For ABI
  inline ~Vec3() {}
};