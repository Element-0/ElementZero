#pragma once

#include <memory>
#include <string>
#include <hook.h>

class Item;
class BlockLegacy;
class CompoundTag;

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class ItemStackBase {
public:
  char filler[128];

  __declspec(dllimport) virtual ~ItemStackBase();

  __declspec(dllimport) short getId() const;
  __declspec(dllimport) int getIdAux() const;
  __declspec(dllimport) unsigned char getMaxStackSize() const;
  __declspec(dllimport) Item const *getItem() const;
  __declspec(dllimport) std::string getName() const;
  __declspec(dllimport) std::string getHoverName() const;
  BASEAPI unsigned char getStackSize() const;

protected:
  virtual void reinit(Item const &, int, int)   = 0;

public:
  virtual void reinit(BlockLegacy const &, int) = 0;
};
class ItemStack : public ItemStackBase {
public:
  __declspec(dllimport) ~ItemStack();

protected:
  __declspec(dllimport) void reinit(Item const &, int, int);

public:
  __declspec(dllimport) void reinit(BlockLegacy const &, int);
};