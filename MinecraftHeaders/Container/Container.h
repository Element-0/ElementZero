#pragma once

#include <unordered_set>
#include <deque>
#include <vector>
#include <functional>

#include "ContainerType.h"
#include "ContainerContentChangeListener.h"
#include "ContainerSizeChangeListener.h"
#include "../Core/NBT.h"
#include "../Item/ItemStack.h"
#include "../Actor/Player.h"
#include "../Math/Vec3.h"

#include "../dll.h"

class BlockSource;
class ItemDescriptor;

class Container {
public:
  ContainerType type;
  std::unordered_set<ContainerContentChangeListener *> content_change_listeners; // 16
  std::unordered_set<ContainerSizeChangeListener *> content_size_listeners;      // 80
  std::deque<std::function<void(Container &, int, ItemStack const &, ItemStack const &)>> transcations;
  std::string custom_name;
  bool has_custom_name;

  MCAPI virtual ~Container();
  MCAPI virtual void addContentChangeListener(ContainerContentChangeListener *);
  MCAPI virtual void removeContentChangeListener(ContainerContentChangeListener *);
  virtual ItemStack const &getItem(int) const = 0;
  MCAPI virtual bool hasRoomForItem(ItemStack const &);
  MCAPI virtual bool addItem(ItemStack &);
  MCAPI virtual bool addItemToFirstEmptySlot(ItemStack &);
  virtual void setItem(int, ItemStack const &) = 0;
  MCAPI virtual void removeItem(int, int);
  MCAPI virtual void removeAllItems();
  MCAPI virtual void dropContents(BlockSource &, Vec3 const &, bool);
  virtual int getContainerSize() const = 0;
  virtual int getMaxStackSize() const  = 0;
  virtual void startOpen(Player &)     = 0;
  virtual void stopOpen(Player &)      = 0;
  MCAPI virtual std::vector<ItemStack> getSlotCopies() const;
  MCAPI virtual std::vector<ItemStack const *> getSlots() const;
  MCAPI virtual int getItemCount(ItemStack const &);
  MCAPI virtual int getItemCount(ItemDescriptor const &);
  MCAPI virtual int findFirstSlotForItem(ItemStack const &);
  MCAPI virtual bool canPushInItem(BlockSource &, int, int, ItemStack const &) const;
  MCAPI virtual bool canPullOutItem(BlockSource &, int, int, ItemStack const &) const;
  MCAPI virtual void setContainerChanged(int);
  MCAPI virtual void setContainerMoved();
  MCAPI virtual void setCustomName(std::string const &);
  MCAPI virtual bool hasCustomName() const;
  MCAPI virtual void readAdditionalSaveData(CompoundTag const &);
  MCAPI virtual void addAdditionalSaveData(CompoundTag &);
  MCAPI virtual void createTransactionContext(
      std::function<void(Container &, int, ItemStack const &, ItemStack const &)>, std::function<void()>);
  MCAPI virtual void triggerTransactionChange(int, ItemStack const &, ItemStack const &);
};

static_assert(offsetof(Container, content_change_listeners) == 16);
static_assert(offsetof(Container, content_size_listeners) == 80);
static_assert(offsetof(Container, custom_name) == 184);