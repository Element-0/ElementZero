#pragma once

#include "Container.h"
#include "ContainerType.h"
#include "../Core/SemVersion.h"
#include "../Level/Level.h"

#include "../dll.h"
#include <memory>

class FillingContainer : public Container {
public:
  std::vector<ItemStack> data;
  Player *player;

  MCAPI FillingContainer(Player *, int, ContainerType);

  virtual bool add(ItemStack &);
  virtual bool canAdd(ItemStack const &);
  virtual void dropAll(bool);
  virtual void clearSlot(int);
  virtual void clearInvertory(int);
  virtual void load(ListTag const &, SemVersion const &, Level &);
  virtual void doDrop(ItemStack &, bool);
  virtual int getEmptySlotsCount() const;
  virtual void dropAll(int, int, bool);

  MCAPI std::unique_ptr<ListTag> save();
};

static_assert(offsetof(FillingContainer, data) == 224);
static_assert(offsetof(FillingContainer, player) == 248);