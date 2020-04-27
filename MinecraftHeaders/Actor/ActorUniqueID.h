#pragma once

#include <cstdint>

struct ActorUniqueID {
public:
  uint64_t value = 0;

  inline ActorUniqueID() {}
  inline ActorUniqueID(uint64_t value) : value(value) {}
};