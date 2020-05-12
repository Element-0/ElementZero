#pragma once

#include "BlockLegacy.h"

class ActorBlock : public BlockLegacy {
public:
  MCAPI virtual ~ActorBlock() override;
  MCAPI virtual bool playerWillDestroy(class Player &, class BlockPos const &, class Block const &) const override;
  MCAPI virtual void
  neighborChanged(class BlockSource &, class BlockPos const &, class BlockPos const &) const override;
  MCAPI virtual void
  spawnResources(class BlockSource &, class BlockPos const &, class Block const &, float, int) const override;
  MCAPI virtual void triggerEvent(class BlockSource &, class BlockPos const &, int, int) const override;
  MCAPI virtual class ItemInstance getEntityResourceItem(class Random &, class BlockActor const &, int) const;

protected:
  MCAPI virtual bool canBeSilkTouched(void) const override;
};