#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../dll.h"

class BlockActor {
public:
  char unknown[200];

  MCAPI virtual ~BlockActor();
  MCAPI virtual void load(class Level &, class CompoundTag const &, class DataLoadHelper &);
  MCAPI virtual bool save(class CompoundTag &) const;
  MCAPI virtual bool saveItemInstanceData(class CompoundTag &);
  MCAPI virtual void saveBlockData(class CompoundTag &, class BlockSource &) const;
  MCAPI virtual void loadBlockData(class CompoundTag const &, class BlockSource &, class DataLoadHelper &);
  MCAPI virtual void onCustomTagLoadDone(class BlockSource &);
  MCAPI virtual void tick(class BlockSource &);
  MCAPI virtual bool isFinished(class BlockSource &);
  MCAPI virtual void onChanged(class BlockSource &);
  MCAPI virtual bool isMovable(class BlockSource &);
  MCAPI virtual bool isCustomNameSaved();
  MCAPI virtual std::unique_ptr<class Packet> getUpdatePacket(class BlockSource &);
  MCAPI virtual void onPlace(class BlockSource &);
  MCAPI virtual void onMove();
  MCAPI virtual void onRemoved(class BlockSource &);
  MCAPI virtual void triggerEvent(int, int);
  MCAPI virtual void clearCache();
  MCAPI virtual void onNeighborChanged(class BlockSource &, class BlockPos const &);
  MCAPI virtual float getShadowRadius(class BlockSource &) const;
  MCAPI virtual bool hasAlphaLayer() const;
  MCAPI virtual class BlockActor *getCrackEntity(class BlockSource &, class BlockPos const &);
  MCAPI virtual void getDebugText(std::vector<std::string> &, class BlockPos const &);
  MCAPI virtual std::string const &getCustomName() const;
  MCAPI virtual std::string const &getFilteredCustomName(class UIProfanityContext const &);
  MCAPI virtual std::string getName() const;
  MCAPI virtual std::string getImmersiveReaderText(class BlockSource &);
  MCAPI virtual int getRepairCost() const;
  MCAPI virtual class PistonBlockActor *getOwningPiston(class BlockSource &);
  MCAPI virtual class Container *getContainer();
  MCAPI virtual float getDeletionDelayTimeSeconds() const;
  MCAPI virtual void onChunkLoaded(class LevelChunk &);
  MCAPI virtual void onChunkUnloaded(class LevelChunk &);

protected:
  MCAPI virtual void _onUpdatePacket(class CompoundTag const &, class BlockSource &);
  MCAPI virtual bool _playerCanUpdate(class Player const &) const;
};

static_assert(sizeof(BlockActor) == 208);