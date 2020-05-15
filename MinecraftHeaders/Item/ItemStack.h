#pragma once

#include <chrono>
#include <memory>
#include <vector>
#include <string>

#include "../Level/Tick.h"

#include "../dll.h"

#include <hook.h>
#include <base.h>

class Item;
class Block;
class BlockLegacy;
class CompoundTag;
class IDataOutput;
class IDataInput;
class ItemInstance;
class ItemEnchants;

class ItemStackBase {
public:
  Item *item{};
  std::unique_ptr<CompoundTag> tag;
  uint64_t block_state{};
  uint16_t aux_value{};
  unsigned char count{};
  bool flag35{};
  std::chrono::steady_clock::time_point time{};
  bool flag48{};
  std::vector<BlockLegacy *> blv56;
  uint64_t unk80{};
  std::vector<BlockLegacy *> blv88;
  uint64_t unk112{};
  Tick blocking_tick;
  std::unique_ptr<ItemInstance> instance;

  MCAPI virtual ~ItemStackBase();

  MCAPI short getId() const;
  MCAPI short getAuxValue() const;
  MCAPI int getIdAux() const;
  MCAPI unsigned char getMaxStackSize() const;
  MCAPI Item const *getItem() const;
  MCAPI bool matchesItem(ItemStackBase const &) const;
  MCAPI bool sameItem(int, int) const;
  MCAPI std::string getRawNameId() const;
  MCAPI std::string getName() const;
  MCAPI std::string getHoverName() const;
  MCAPI bool hasCustomHoverName() const;
  MCAPI bool isStackable(ItemStackBase const &) const;
  MCAPI bool isStackedByData(void) const;
  MCAPI bool isDamaged() const;
  MCAPI bool isBlock() const;
  MCAPI bool isEnchanted() const;
  MCAPI bool isHorseArmorItem() const;
  MCAPI bool isPotionItem() const;
  MCAPI bool isWearableItem() const;
  MCAPI bool isExperimental() const;
  MCAPI bool isNull() const;
  MCAPI void setNull();
  MCAPI void set(int);
  MCAPI void setRepairCost(int);
  MCAPI void setChargedItem(ItemInstance const &, bool);
  MCAPI void setCustomLore(std::vector<std::string> const &lore);
  MCAPI void setCustomName(std::string const &name);
  MCAPI void clearChargedItem();
  MCAPI void serializeComponents(IDataOutput &) const;
  MCAPI void deserializeComponents(IDataInput &) const;
  MCAPI ItemEnchants getEnchantsFromUserData() const;
  inline unsigned char getStackSize() const { return count; }
  MCAPI std::unique_ptr<CompoundTag> save() const;
  MCAPI std::string toString() const;

  MCAPI bool operator!=(ItemStackBase const &rhs) const;
  MCAPI operator bool() const;

protected:
  virtual void reinit(Item const &, int, int) = 0;
  MCAPI ItemStackBase();
  MCAPI ItemStackBase(Item const &item);
  MCAPI ItemStackBase(Item const &ite, int);
  MCAPI ItemStackBase(Item const &ite, int, int);
  MCAPI ItemStackBase(Item const &ite, int, int, CompoundTag const *);
  MCAPI ItemStackBase(BlockLegacy const &, int);
  MCAPI ItemStackBase(Block const &, int, CompoundTag const *);
  MCAPI ItemStackBase(ItemStackBase const &rhs);
  MCAPI ItemStackBase &operator=(ItemStackBase const &rhs);

public:
  virtual void reinit(BlockLegacy const &, int) = 0;
};
class ItemStack : public ItemStackBase {
public:
  MCAPI ItemStack();
  ItemStack(Item const &item) : ItemStackBase(item) {}
  MCAPI ItemStack(Item const &, int);
  ItemStack(Item const &item, int a, int b) : ItemStackBase(item, a, b) {}
  ItemStack(Item const &item, int a, int b, CompoundTag const *tag) : ItemStackBase(item, a, b, tag) {}
  ItemStack(BlockLegacy const &block, int a) : ItemStackBase(block, a) {}
  ItemStack(Block const &block, int a, CompoundTag const *tag) : ItemStackBase(block, a, tag) {}
  ItemStack(ItemStack const &rhs) : ItemStackBase(rhs) {}
  MCAPI ~ItemStack();
  MCAPI static ItemStack fromTag(CompoundTag const &);

  ItemStack &operator=(ItemStack const &rhs) {
    ItemStackBase::operator=(rhs);
    return *this;
  }

protected:
  MCAPI void reinit(Item const &, int, int);

public:
  MCAPI void reinit(BlockLegacy const &, int);
};

static_assert(offsetof(ItemStackBase, time) == 40);
static_assert(sizeof(ItemStackBase) == 136);