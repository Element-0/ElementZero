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
  MCAPI ~FillingContainer() override;
  MCAPI virtual ItemStack const &getItem(int) const override;
  MCAPI virtual void setItem(int, ItemStack const &) override;
  MCAPI virtual int getContainerSize() const override;
  MCAPI virtual int getMaxStackSize() const override;
  MCAPI virtual void startOpen(Player &) override;
  MCAPI virtual void stopOpen(Player &) override;

  MCAPI virtual bool add(ItemStack &);
  MCAPI virtual bool canAdd(ItemStack const &);
  MCAPI virtual void dropAll(bool);
  MCAPI virtual void clearSlot(int);
  MCAPI virtual void clearInvertory(int);
  MCAPI virtual void load(ListTag const &, SemVersion const &, Level &);
  MCAPI virtual void doDrop(ItemStack &, bool);
  MCAPI virtual int getEmptySlotsCount() const;
  MCAPI virtual void dropAll(int, int, bool);

  MCAPI std::unique_ptr<ListTag> save();
};

static_assert(offsetof(FillingContainer, data) == 224);
static_assert(offsetof(FillingContainer, player) == 248);