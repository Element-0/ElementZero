#pragma once

class BlockPos {
public:
  int x, y, z;
  // For ABI
  inline ~BlockPos() {}
};