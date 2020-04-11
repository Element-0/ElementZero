#pragma once

#include <memory>
#include <string>
#include <hook.h>

#include "../dll.h"

class Item;
class BlockLegacy;
class CompoundTag;
class IDataOutput;
class IDataInput;
class ItemInstance;

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class ItemStackBase {
public:
  char filler[128];

  MCAPI virtual ~ItemStackBase();

  MCAPI short getId() const;
  MCAPI short getAuxValue() const;
  MCAPI int getIdAux() const;
  MCAPI unsigned char getMaxStackSize() const;
  MCAPI Item const *getItem() const;
  MCAPI bool matchesItem(ItemStackBase const &) const;
  MCAPI bool sameItem(int, int) const;
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
  MCAPI void clearChargedItem();
  MCAPI void serializeComponents(IDataOutput &) const;
  MCAPI void deserializeComponents(IDataInput &) const;
  BASEAPI unsigned char getStackSize() const;

protected:
  virtual void reinit(Item const &, int, int) = 0;

public:
  virtual void reinit(BlockLegacy const &, int) = 0;
};
class ItemStack : public ItemStackBase {
public:
  MCAPI ~ItemStack();

protected:
  MCAPI void reinit(Item const &, int, int);

public:
  MCAPI void reinit(BlockLegacy const &, int);
};