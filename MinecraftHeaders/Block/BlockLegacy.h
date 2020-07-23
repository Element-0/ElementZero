#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#include "../dll.h"

#include <modutils.h>

class BlockLegacy {
public:
  MCAPI virtual ~BlockLegacy();
  MCAPI virtual void tick(class BlockSource &, class BlockPos const &, class Random &) const;
  MCAPI virtual class Block const &getStateFromLegacyData(unsigned short) const;
  MCAPI virtual class Block const *getNextBlockPermutation(class Block const &) const;
  MCAPI virtual bool getCollisionShape(
      class AABB &, class Block const &, class BlockSource &, class BlockPos const &, class Actor *) const;
  MCAPI virtual bool isObstructingChests(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual class Vec3 randomlyModifyPosition(class BlockPos const &, int &) const;
  MCAPI virtual class Vec3 randomlyModifyPosition(class BlockPos const &) const;
  MCAPI virtual void addAABBs(
      class Block const &, class BlockSource &, class BlockPos const &, class AABB const *,
      std::vector<class AABB> &) const;
  MCAPI virtual class AABB const &
  getAABB(class BlockSource &, class BlockPos const &, class Block const &, class AABB &, bool) const;
  MCAPI virtual bool addCollisionShapes(
      class Block const &, class BlockSource &, class BlockPos const &, class AABB const *, std::vector<class AABB> &,
      Actor *) const;
  MCAPI virtual class AABB const &getOutline(class BlockSource &, class BlockPos const &, class AABB &) const;
  MCAPI virtual bool getLiquidClipVolume(class BlockSource &, class BlockPos const &, class AABB &) const;
  MCAPI virtual void onProjectileHit(class BlockSource &, class BlockPos const &, class Actor const &) const;
  MCAPI virtual bool liquidCanFlowIntoFromDirection(
      unsigned char, std::function<class Block const &(class BlockPos const &)> const &, class BlockPos const &) const;
  MCAPI virtual struct Brightness getLightEmission(class Block const &) const;
  MCAPI virtual bool shouldRandomTick() const;
  MCAPI virtual bool hasVariableLighting() const;
  MCAPI virtual bool isStrippable(class Block const &) const;
  MCAPI virtual class Block const &getStrippedBlock(class Block const &) const;
  MCAPI virtual bool canProvideSupport(class Block const &, unsigned char, enum BlockSupportType) const;
  MCAPI virtual bool canConnect(class Block const &, unsigned char, class Block const &) const;
  MCAPI virtual void getConnectedDirections(
      class Block const &, class BlockPos const &, class BlockSource &, bool &, bool &, bool &, bool &) const;
  MCAPI virtual bool isCropBlock() const;
  MCAPI virtual bool isStemBlock() const;
  MCAPI virtual bool isContainerBlock() const;
  MCAPI virtual bool isCraftingBlock() const;
  MCAPI virtual bool isInteractiveBlock() const;
  MCAPI virtual bool isWaterBlocking() const;
  MCAPI virtual bool isHurtableBlock() const;
  MCAPI virtual bool isFenceBlock() const;
  MCAPI virtual bool isFenceGateBlock() const;
  MCAPI virtual bool isThinFenceBlock() const;
  MCAPI virtual bool isWallBlock() const;
  MCAPI virtual bool isStairBlock() const;
  MCAPI virtual bool isSlabBlock() const;
  MCAPI virtual bool isDoorBlock() const;
  MCAPI virtual bool isRailBlock() const;
  MCAPI virtual bool isButtonBlock() const;
  MCAPI virtual bool canHurtAndBreakItem() const;
  MCAPI virtual bool isSignalSource() const;
  MCAPI virtual bool canBeOriginalSurface() const;
  MCAPI virtual bool isValidAuxValue() const;
  MCAPI virtual bool canFillAtPos(class BlockSource &, class BlockPos const &, class Block const &) const;
  MCAPI virtual class Block const &
  sanitizeFillBlock(class BlockSource &, class BlockPos const &, class Block const &) const;
  MCAPI virtual void onFillBlock(class BlockSource &, class BlockPos const &, class Block const &) const;
  MCAPI virtual int getDirectSignal(class BlockSource &, class BlockPos const &, int) const;
  MCAPI virtual bool waterSpreadCausesSpawn() const;
  MCAPI virtual bool canContainLiquid() const;
  MCAPI virtual bool shouldConnectToRedstone(class BlockSource &, class BlockPos const &, int) const;
  MCAPI virtual void handleRain(class BlockSource &, class BlockPos const &, float) const;
  MCAPI virtual bool canBeUsedInCommands(bool, class BaseGameVersion const &) const;
  MCAPI virtual float getThickness() const;
  MCAPI virtual float getFlexibility(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual bool checkIsPathable(class Actor &, class BlockPos const &, class BlockPos const &) const;
  MCAPI virtual bool shouldDispense(class BlockSource &, class Container &) const;
  MCAPI virtual bool dispense(class BlockSource &, class Container &, int, class Vec3 const &, unsigned char) const;
  MCAPI virtual void onPlace(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual void onRemove(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual void onExploded(class BlockSource &, class BlockPos const &, class Actor *) const;
  MCAPI virtual void onStepOn(class Actor &, class BlockPos const &) const;
  MCAPI virtual void onFallOn(class BlockSource &, class BlockPos const &, class Actor *, float) const;
  MCAPI virtual void transformOnFall(class BlockSource &, class BlockPos const &, class Actor *, float) const;
  MCAPI virtual void onRedstoneUpdate(class BlockSource &, class BlockPos const &, int, bool) const;
  MCAPI virtual void onMove(class BlockSource &, class BlockPos const &, class BlockPos const &) const;
  MCAPI virtual bool detachesOnPistonMove(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual void movedByPiston(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual void setupRedstoneComponent(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual enum BlockProperty getRedstoneProperty(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual void updateEntityAfterFallOn(class Actor &) const;
  MCAPI virtual bool isBounceBlock() const;
  MCAPI virtual bool ignoreEntitiesOnPistonMove(class Block const &) const;
  MCAPI virtual bool
  onFertilized(class BlockSource &, class BlockPos const &, class Actor *, enum FertilizerType) const;
  MCAPI virtual bool mayConsumeFertilizer(class BlockSource &) const;
  MCAPI virtual bool mayPick() const;
  MCAPI virtual bool mayPick(class BlockSource &, class Block const &, bool) const;
  MCAPI virtual bool mayPlace(class BlockSource &, class BlockPos const &, unsigned char) const;
  MCAPI virtual bool mayPlace(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual bool mayPlaceOn(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual bool tryToPlace(
      class BlockSource &, class BlockPos const &, class Block const &, struct ActorBlockSyncMessage const *) const;
  MCAPI virtual bool breaksFallingBlocks(class Block const &) const;
  MCAPI virtual void destroy(class BlockSource &, class BlockPos const &, class Block const &, class Actor *) const;
  MCAPI virtual bool playerWillDestroy(class Player &, class BlockPos const &, class Block const &) const;
  MCAPI virtual bool getIgnoresDestroyPermissions(class Actor &, class BlockPos const &) const;
  MCAPI virtual void neighborChanged(class BlockSource &, class BlockPos const &, class BlockPos const &) const;
  MCAPI virtual bool getSecondPart(class BlockSource &, class BlockPos const &, class BlockPos &) const;
  MCAPI virtual int getResourceCount(class Random &, class Block const &, int) const;
  MCAPI virtual class ItemInstance getResourceItem(class Random &, class Block const &, int) const;
  MCAPI virtual class ItemInstance
  asItemInstance(class BlockSource &, class BlockPos const &, class Block const &) const;
  MCAPI virtual void spawnResources(class BlockSource &, class BlockPos const &, class Block const &, float, int) const;
  MCAPI virtual bool spawnBurnResources(class BlockSource &, float, float, float) const;
  MCAPI virtual float getExplosionResistance(class Actor *) const;
  MCAPI virtual class HitResult
  clip(class BlockSource &, class BlockPos const &, class Vec3 const &, class Vec3 const &, bool) const;
  MCAPI virtual bool use(class Player &, class BlockPos const &) const;
  MCAPI virtual class Block const &
  getPlacementBlock(class Actor &, class BlockPos const &, unsigned char, class Vec3 const &, int) const;
  MCAPI virtual int calcVariant(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual bool isAttachedTo(class BlockSource &, class BlockPos const &, class BlockPos &) const;
  MCAPI virtual bool attack(class Player *, class BlockPos const &) const;
  MCAPI virtual void handleEntityInside(class BlockSource &, class BlockPos const &, class Actor *, class Vec3 &) const;
  MCAPI virtual void entityInside(class BlockSource &, class BlockPos const &, class Actor &) const;
  MCAPI virtual void playerDestroy(class Player &, class BlockPos const &, class Block const &) const;
  MCAPI virtual bool canSurvive(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual int getExperienceDrop(class Random &) const;
  MCAPI virtual bool canBeBuiltOver(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual void triggerEvent(class BlockSource &, class BlockPos const &, int, int) const;
  MCAPI virtual void executeEvent(
      class BlockSource &, class BlockPos const &, class Block const &, std::string const &, class Actor &) const;
  MCAPI virtual bool
  hasTag(class BlockSource &, class BlockPos const &, class Block const &, std::string const &) const;
  MCAPI virtual class MobSpawnerData const *getMobToSpawn(class SpawnConditions const &, class BlockSource &) const;
  MCAPI virtual class Color getMapColor(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual class Color getMapColor() const;
  MCAPI virtual bool shouldStopFalling(class Actor &) const;
  MCAPI virtual bool pushesUpFallingBlocks() const;
  MCAPI virtual float calcGroundFriction(class Mob &, class BlockPos const &) const;
  MCAPI virtual bool canHaveExtraData() const;
  MCAPI virtual bool hasComparatorSignal() const;
  MCAPI virtual int
  getComparatorSignal(class BlockSource &, class BlockPos const &, class Block const &, unsigned char) const;
  MCAPI virtual bool canSlide(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual int getIconYOffset() const;
  MCAPI virtual enum BlockRenderLayer getRenderLayer() const;
  MCAPI virtual std::string buildDescriptionId(class Block const &) const;
  MCAPI virtual bool isAuxValueRelevantForPicking() const;
  MCAPI virtual int getColor(class Block const &) const;
  MCAPI virtual int getColorAtPos(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual int getColor(class BlockSource &, class BlockPos const &, class Block const &) const;
  MCAPI virtual int getColorForParticle(class BlockSource &, class BlockPos const &, class Block const &) const;
  MCAPI virtual bool isSeasonTinted(class Block const &, class BlockSource &, class BlockPos const &) const;
  MCAPI virtual void onGraphicsModeChanged(bool, bool, bool);
  MCAPI virtual enum BlockRenderLayer
  getRenderLayer(class Block const &, class BlockSource &, class BlockPos const &) const;
  MCAPI virtual int getExtraRenderLayers() const;
  MCAPI virtual float getShadeBrightness() const;
  MCAPI virtual class AABB const &
  getVisualShapeInWorld(class Block const &, class BlockSource &, class BlockPos const &, class AABB &, bool) const;
  MCAPI virtual class AABB const &getVisualShape(class Block const &, class AABB &, bool) const;
  MCAPI virtual class AABB const &getUIShape(class Block const &, class AABB &) const;
  MCAPI virtual int telemetryVariant(class BlockSource &, class BlockPos const &) const;
  MCAPI virtual int getVariant(class Block const &) const;
  MCAPI virtual bool canSpawnOn() const;
  MCAPI virtual unsigned char getMappedFace(unsigned char, class Block const &) const;
  MCAPI virtual enum Flip getFaceFlip(unsigned char, class Block const &) const;
  MCAPI virtual void animateTick(class BlockSource &, class BlockPos const &, class Random &) const;
  MCAPI virtual class BlockLegacy &init(void);
  MCAPI virtual class BlockLegacy &setLightBlock(struct Brightness);
  MCAPI virtual class BlockLegacy &setLightEmission(float);
  MCAPI virtual class BlockLegacy &setExplodeable(float);
  MCAPI virtual class BlockLegacy &setDestroyTime(float);
  MCAPI virtual class BlockLegacy &setFriction(float);
  MCAPI virtual class BlockLegacy &addProperty(enum BlockProperty);
  MCAPI virtual class BlockLegacy &addState(class ItemState const &);
  MCAPI virtual class BlockLegacy &addState(class ItemState const &, uint64_t);
  MCAPI virtual class BlockLegacy &setAllowsRunes(bool);
  MCAPI virtual class BlockLegacy &setMapColor(class Color const &);
  MCAPI virtual bool canBeSilkTouched(void) const;
  MCAPI virtual class ItemInstance getSilkTouchItemInstance(class Block const &) const;
  MCAPI virtual void setVisualShape(class AABB const &);
  MCAPI virtual void setVisualShape(class Vec3 const &, class Vec3 const &);
  MCAPI virtual class Block const *tryLegacyUpgrade(unsigned short) const;
  MCAPI virtual bool dealsContactDamage(class Actor const &, class Block const &, bool) const;
  
  BUILD_ACCESS(unsigned short, BlockID, 268);
  BUILD_ACCESS(class Block**,Block, 514);
  template <typename T> MCAPI T getState(class ItemState const &,unsigned short) const;
  class Block const &getDefaultState(void)const;
};