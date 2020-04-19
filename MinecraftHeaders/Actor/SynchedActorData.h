#pragma once

#include <vector>
#include <memory>

#include "../Core/DataItem.h"

class SynchedActorData {
public:
  std::vector<std::unique_ptr<DataItem>> items;
  unsigned short start, end;

  template <typename T> void append(DataItem::Id id, T const &value) {
    items.emplace_back(std::make_unique<DataItem2<T>>(id, value));
  }
};

static_assert(offsetof(SynchedActorData, start) == 24);
static_assert(offsetof(SynchedActorData, end) == 26);