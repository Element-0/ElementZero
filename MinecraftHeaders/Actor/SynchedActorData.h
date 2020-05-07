#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "../Core/DataItem.h"

class SynchedActorData {
public:
  std::vector<std::unique_ptr<DataItem>> items;
  DataItem::Id start, end;

  void _setDirty(DataItem::Id pos) {
    start = (DataItem::Id) std::min((unsigned short) pos, (unsigned short) start);
    end   = (DataItem::Id) std::max((unsigned short) pos, (unsigned short) end);
  }

  DataItem &_get(DataItem::Id pos) { return *items[(unsigned int) pos]; }
  DataItem *_find(DataItem::Id pos) {
    return items.size() > (unsigned int) pos ? &*items[(unsigned int) pos] : nullptr;
  }

  bool isDirty() const { return start <= end; }

  void markDirty(DataItem &item) {
    item.setDirty();
    _setDirty(item.getId());
  }

  void markDirty(DataItem::Id id) {}

  template <typename T> void append(DataItem::Id id, T const &value) {
    items.emplace_back(std::make_unique<DataItem2<T>>(id, value));
  }

  template <typename T> void set(DataItem *item, T const &value) {
    if (item) {
      if (item->type == DataTypeMap::typeFor<T>) {
        auto titem = static_cast<DataItem2<T> *>(item);
        if (DataTypeMap::neqFor<T>::neq(titem->value, value)) {
          DataTypeMap::copyFor<T>::copy(titem->value, value);
          markDirty(*item);
        }
      }
    }
  }
  template <typename T> void set(DataItem::Id id, T const &value) { set<T>(_find(id), value); }
};

static_assert(offsetof(SynchedActorData, start) == 24);
static_assert(offsetof(SynchedActorData, end) == 26);