#pragma once

#include <memory>
#include <string>
#include <hook.h>

#include "../dll.h"

class Item;
class BlockLegacy;
class CompoundTag;

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
  MCAPI std::string getName() const;
  MCAPI std::string getHoverName() const;
  BASEAPI unsigned char getStackSize() const;

protected:
  virtual void reinit(Item const &, int, int)   = 0;

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