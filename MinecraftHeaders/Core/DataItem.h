#pragma once

#include <cstdint>

#include "../dll.h"

enum class DataItemType : unsigned char {};

class DataItem {
  DataItemType type;
  uint16_t id;
  bool dirty;

  inline DataItemType getType() const { return type; }
  inline uint64_t getId() const { return id; }
  inline bool isDirty() const { return dirty; }
  inline void setDirty(bool d) { dirty = d; }
  MCAPI bool isDataEqual(DataItem const &rhs) const;
};