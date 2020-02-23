#pragma once

#include <memory>
#include <string>

class Item;
class BlockLegacy;
class CompoundTag;

class ItemStackBase {
public:
  char filler[128];

  __declspec(dllimport) virtual ~ItemStackBase();
  virtual void reinit(Item const &, int, int)   = 0;
  virtual void reinit(BlockLegacy const &, int) = 0;

  __declspec(dllimport) short getId() const;
  __declspec(dllimport) int getIdAux() const;
  __declspec(dllimport) unsigned char getMaxStackSize() const;
  __declspec(dllimport) Item const *getItem() const;
  __declspec(dllimport) std::string getName() const;
  __declspec(dllimport) std::string getHoverName() const;
};
class ItemStack : public ItemStackBase {
public:
  __declspec(dllimport) ~ItemStack();
  __declspec(dllimport) void reinit(Item const &, int, int);
  __declspec(dllimport) void reinit(BlockLegacy const &, int);
};