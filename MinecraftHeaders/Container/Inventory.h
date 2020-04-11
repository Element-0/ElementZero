#pragma once

#include "FillingContainer.h"

class Inventory : public FillingContainer {
public:
  Inventory(Player *);
  MCAPI virtual void init() override;
  MCAPI virtual bool add(ItemStack &) override;
  MCAPI virtual bool canAdd(ItemStack const &) override;
  MCAPI virtual void dropAll(bool) override;
  MCAPI virtual int getEmptySlotsCount() const override;
  MCAPI virtual int getFirstEmptySlot() const;
  MCAPI virtual void setContainerSize(int);
  MCAPI void tick();

private:
  MCAPI void setupDefault();
};