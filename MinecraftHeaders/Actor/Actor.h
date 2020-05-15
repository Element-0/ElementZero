#pragma once

#include <cstdint>

#include "ActorRuntimeID.h"
#include "ActorType.h"
#include "ActorUniqueID.h"
#include "SynchedActorData.h"
#include "../Math/Vec3.h"
#include "../Container/SimpleContainer.h"
#include "../Core/AutomaticID.h"
#include "../Core/Util.h"
#include "../Command/CommandPermissionLevel.h"
#include "../dll.h"

#include <base.h>
#include <hook.h>
#include <modutils.h>

class Dimension;

class Actor {

public:
  MCAPI virtual void reloadHardcoded(enum InitializationMethod, class VariantParameterList const &);
  MCAPI virtual void reloadHardcodedClient(enum InitializationMethod, class VariantParameterList const &);
  MCAPI virtual void initializeComponents(enum InitializationMethod, class VariantParameterList const &);
  MCAPI virtual void reloadComponents(enum InitializationMethod, class VariantParameterList const &);
  MCAPI virtual bool hasComponent(class Util::HashString const &) const;
  MCAPI virtual ~Actor();
  MCAPI virtual void reset(void);
  MCAPI virtual int getOnDeathExperience(void);
  MCAPI virtual enum ActorType getOwnerEntityType(void);
  MCAPI virtual void remove(void);
  MCAPI virtual void setPos(class Vec3 const &);
  MCAPI virtual class Vec3 const &getPos(void) const;
  MCAPI virtual class Vec3 const &getPosOld(void) const;
  MCAPI virtual class Vec3 const getPosExtrapolated(float) const;
  MCAPI virtual class Vec3 getAttachPos(enum ActorLocation, float) const;
  MCAPI virtual class Vec3 getFiringPos(void) const;
  MCAPI virtual void setRot(class Vec2 const &);
  MCAPI virtual void move(class Vec3 const &);
  MCAPI virtual class Vec3 getInterpolatedRidingPosition(float) const;
  MCAPI virtual float getInterpolatedBodyRot(float) const;
  MCAPI virtual float getInterpolatedHeadRot(float) const;
  MCAPI virtual float getInterpolatedBodyYaw(float) const;
  MCAPI virtual float getYawSpeedInDegreesPerSecond(void) const;
  MCAPI virtual float getInterpolatedWalkAnimSpeed(float) const;
  MCAPI virtual class Vec3 getInterpolatedRidingOffset(float) const;
  MCAPI virtual void checkBlockCollisions(class AABB const &);
  MCAPI virtual void checkBlockCollisions(void);
  MCAPI virtual bool breaksFallingBlocks(void) const;
  MCAPI virtual void blockedByShield(class ActorDamageSource const &, class Actor &);
  MCAPI virtual void moveRelative(float, float, float, float);
  MCAPI virtual void teleportTo(class Vec3 const &, bool, int, int);
  MCAPI virtual bool tryTeleportTo(class Vec3 const &, bool, bool, int, int);
  MCAPI virtual void chorusFruitTeleport(class Vec3 &);
  MCAPI virtual void lerpTo(class Vec3 const &, class Vec2 const &, int);
  MCAPI virtual void lerpMotion(class Vec3 const &);
  MCAPI virtual std::unique_ptr<class Packet> getAddPacket(void);
  MCAPI virtual void normalTick(void);
  MCAPI virtual void baseTick(void);
  MCAPI virtual void rideTick(void);
  MCAPI virtual void positionRider(class Actor &, float);
  MCAPI virtual float getRidingHeight(void);
  MCAPI virtual bool startRiding(class Actor &);
  MCAPI virtual void addRider(class Actor &);
  MCAPI virtual void flagRiderToRemove(class Actor &);
  MCAPI virtual std::string getExitTip(std::string const &, enum InputMode) const;
  MCAPI virtual bool intersects(class Vec3 const &, class Vec3 const &) const;
  MCAPI virtual bool isFree(class Vec3 const &, float);
  MCAPI virtual bool isFree(class Vec3 const &);
  MCAPI virtual bool isInWall(void) const;
  MCAPI virtual bool isInvisible(void) const;
  MCAPI virtual bool canShowNameTag(void) const;
  MCAPI virtual bool canExistInPeaceful(void) const;
  MCAPI virtual void setNameTagVisible(bool);
  MCAPI virtual std::string const &getNameTag(void) const;
  MCAPI virtual uint64_t getNameTagAsHash(void) const;
  MCAPI virtual std::string getFormattedNameTag(void) const;
  MCAPI virtual void filterFormattedNameTag(class UIProfanityContext const &);
  MCAPI virtual void setNameTag(std::string const &);
  MCAPI virtual bool getAlwaysShowNameTag(void) const;
  MCAPI virtual void setScoreTag(std::string const &);
  MCAPI virtual std::string const &getScoreTag(void) const;
  MCAPI virtual bool isInWater(void) const;
  MCAPI virtual bool hasEnteredWater(void) const;
  MCAPI virtual bool isImmersedInWater(void) const;
  MCAPI virtual bool isInWaterOrRain(void) const;
  MCAPI virtual bool isInLava(void) const;
  MCAPI virtual bool isUnderLiquid(enum MaterialType) const;
  MCAPI virtual bool isOverWater(void) const;
  MCAPI virtual void makeStuckInBlock(float);
  MCAPI virtual float getCameraOffset(void) const;
  MCAPI virtual float getShadowHeightOffs(void);
  MCAPI virtual float getShadowRadius(void) const;
  MCAPI virtual class Vec3 getHeadLookVector(float);
  MCAPI virtual bool canSeeInvisible(void) const;
  MCAPI virtual bool canSee(class Actor const &) const;
  MCAPI virtual bool canSee(class Vec3 const &) const;
  MCAPI virtual bool isSkyLit(float);
  MCAPI virtual float getBrightness(float) const;
  MCAPI virtual bool interactPreventDefault(void);
  MCAPI virtual void playerTouch(class Player &);
  MCAPI virtual void onAboveBubbleColumn(bool);
  MCAPI virtual void onInsideBubbleColumn(bool);
  MCAPI virtual bool isImmobile(void) const;
  MCAPI virtual bool isSilent(void);
  MCAPI virtual bool isPickable(void);
  MCAPI virtual bool isFishable(void) const;
  MCAPI virtual bool isSleeping(void) const;
  MCAPI virtual bool isShootable(void);
  MCAPI virtual bool isSneaking(void) const;
  MCAPI virtual void setSneaking(bool);
  MCAPI virtual bool isBlocking(void) const;
  MCAPI virtual bool isDamageBlocked(class ActorDamageSource const &) const;
  MCAPI virtual bool isAlive(void) const;
  MCAPI virtual bool isOnFire(void) const;
  MCAPI virtual bool isOnHotBlock(void) const;
  MCAPI virtual bool isCreativeModeAllowed(void);
  MCAPI virtual bool isSurfaceMob(void) const;
  MCAPI virtual bool isTargetable(void) const;
  MCAPI virtual bool canAttack(class Actor *, bool) const;
  MCAPI virtual void setTarget(class Actor *);
  MCAPI virtual class Actor *findAttackTarget(void);
  MCAPI virtual bool isValidTarget(class Actor *) const;
  MCAPI virtual bool attack(class Actor &);
  MCAPI virtual void performRangedAttack(class Actor &, float);
  MCAPI virtual void adjustDamageAmount(int &) const;
  MCAPI virtual int getEquipmentCount(void) const;
  MCAPI virtual void setOwner(struct ActorUniqueID);
  MCAPI virtual void setSitting(bool);
  MCAPI virtual void onTame(void);
  MCAPI virtual void onFailedTame(void);
  MCAPI virtual int getInventorySize(void) const;
  MCAPI virtual int getEquipSlots(void) const;
  MCAPI virtual int getChestSlots(void) const;
  MCAPI virtual void setStanding(bool);
  MCAPI virtual bool canPowerJump(void) const;
  MCAPI virtual void setCanPowerJump(bool);
  MCAPI virtual bool isJumping(void) const;
  MCAPI virtual bool isEnchanted(void) const;
  MCAPI virtual void rideJumped(void);
  MCAPI virtual void rideLanded(class Vec3 const &, class Vec3 const &);
  MCAPI virtual bool shouldRender(void) const;
  MCAPI virtual bool isInvulnerableTo(class ActorDamageSource const &) const;
  MCAPI virtual void actuallyHurt(int, class ActorDamageSource const *, bool);
  MCAPI virtual void animateHurt(void);
  MCAPI virtual bool doFireHurt(int);
  MCAPI virtual void onLightningHit(void);
  MCAPI virtual void onBounceStarted(class BlockPos const &, class Block const &);
  MCAPI virtual void feed(int);
  MCAPI virtual void handleEntityEvent(enum ActorEvent, int);
  MCAPI virtual float getPickRadius(void);
  MCAPI virtual class HashedString const &getActorRendererId(void) const;
  MCAPI virtual class ItemActor *spawnAtLocation(int, int);
  MCAPI virtual class ItemActor *spawnAtLocation(int, int, float);
  MCAPI virtual class ItemActor *spawnAtLocation(class Block const &, int);
  MCAPI virtual class ItemActor *spawnAtLocation(class Block const &, int, float);
  MCAPI virtual class ItemActor *spawnAtLocation(class ItemStack const &, float);
  MCAPI virtual void despawn(void);
  MCAPI virtual void killed(class Actor &);
  MCAPI virtual void awardKillScore(class Actor &, int);
  MCAPI virtual void setArmor(enum ArmorSlot, class ItemStack const &);
  MCAPI virtual class ItemStack const &getArmor(enum ArmorSlot) const;
  MCAPI virtual enum ArmorMaterialType getArmorMaterialTypeInSlot(enum ArmorSlot) const;
  MCAPI virtual enum ArmorTextureType getArmorMaterialTextureTypeInSlot(enum ArmorSlot) const;
  MCAPI virtual float getArmorColorInSlot(enum ArmorSlot, int) const;
  MCAPI virtual void setEquippedSlot(enum ArmorSlot, int, int);
  MCAPI virtual void setEquippedSlot(enum ArmorSlot, class ItemStack const &);
  MCAPI virtual class ItemStack const &getCarriedItem(void) const;
  MCAPI virtual void setCarriedItem(class ItemStack const &);
  MCAPI virtual void setOffhandSlot(class ItemStack const &);
  MCAPI virtual class ItemStack const &getEquippedTotem(void) const;
  MCAPI virtual bool consumeTotem(void);
  MCAPI virtual bool save(class CompoundTag &);
  MCAPI virtual void saveWithoutId(class CompoundTag &);
  MCAPI virtual bool load(class CompoundTag const &, class DataLoadHelper &);
  MCAPI virtual void loadLinks(class CompoundTag const &, std::vector<struct ActorLink> &, class DataLoadHelper &);
  MCAPI virtual enum ActorType getEntityTypeId(void) const;
  MCAPI virtual class HashedString const &queryEntityRenderer(void) const;
  MCAPI virtual struct ActorUniqueID getSourceUniqueID(void) const;
  MCAPI virtual void setOnFire(int);
  MCAPI virtual class AABB getHandleWaterAABB(void) const;
  MCAPI virtual void handleInsidePortal(class BlockPos const &);
  MCAPI virtual int getPortalCooldown(void) const;
  MCAPI virtual int getPortalWaitTime(void) const;
  MCAPI virtual class AutomaticID<class Dimension, int> getDimensionId(void) const;
  MCAPI virtual bool canChangeDimensions(void) const;
  MCAPI virtual void changeDimension(class AutomaticID<class Dimension, int>, bool);
  MCAPI virtual void changeDimension(class ChangeDimensionPacket const &);
  MCAPI virtual struct ActorUniqueID getControllingPlayer(void) const;
  MCAPI virtual void checkFallDamage(float, bool);
  MCAPI virtual void causeFallDamage(float);
  MCAPI virtual void handleFallDistanceOnServer(float, bool);
  MCAPI virtual void playSynchronizedSound(enum LevelSoundEvent, class Vec3 const &, class Block const &, bool);
  MCAPI virtual void playSynchronizedSound(enum LevelSoundEvent, class Vec3 const &, int, bool);
  MCAPI virtual void onSynchedDataUpdate(int);
  MCAPI virtual bool canAddRider(class Actor &) const;
  MCAPI virtual bool canBePulledIntoVehicle(void) const;
  MCAPI virtual bool inCaravan(void) const;
  MCAPI virtual bool isLeashableType(void);
  MCAPI virtual void tickLeash(void);
  MCAPI virtual void sendMotionPacketIfNeeded(void);
  MCAPI virtual bool canSynchronizeNewEntity(void) const;
  MCAPI virtual void stopRiding(bool, bool, bool);
  MCAPI virtual void startSwimming(void);
  MCAPI virtual void stopSwimming(void);
  MCAPI virtual void buildDebugInfo(std::string &) const;
  MCAPI virtual enum CommandPermissionLevel getCommandPermissionLevel(void) const;
  MCAPI virtual class AttributeInstance *getMutableAttribute(class Attribute const &);
  MCAPI virtual class AttributeInstance const &getAttribute(class Attribute const &) const;
  MCAPI virtual int getDeathTime(void) const;
  MCAPI virtual void heal(int);
  MCAPI virtual bool isInvertedHealAndHarm(void) const;
  MCAPI virtual bool canBeAffected(int) const;
  MCAPI virtual bool canBeAffected(class MobEffectInstance const &) const;
  MCAPI virtual bool canBeAffectedByArrow(class MobEffectInstance const &) const;
  MCAPI virtual void onEffectAdded(class MobEffectInstance &);
  MCAPI virtual void onEffectUpdated(class MobEffectInstance const &);
  MCAPI virtual void onEffectRemoved(class MobEffectInstance &);
  MCAPI virtual class AnimationComponent &getAnimationComponent(void);
  MCAPI virtual void openContainerComponent(class Player &);
  MCAPI virtual void swing(void);
  MCAPI virtual void useItem(class ItemStack &, enum ItemUseMethod, bool);
  MCAPI virtual bool hasOutputSignal(unsigned char) const;
  MCAPI virtual int getOutputSignal(void) const;
  MCAPI virtual void getDebugText(std::vector<std::string> &);
  MCAPI virtual float getMapDecorationRotation(void) const;
  MCAPI virtual float getRiderYRotation(class Actor const &) const;
  MCAPI virtual float getYHeadRot(void) const;
  MCAPI virtual bool isWorldBuilder(void);
  MCAPI virtual bool isCreative(void) const;
  MCAPI virtual bool isAdventure(void) const;
  MCAPI virtual bool add(class ItemStack &);
  MCAPI virtual bool drop(class ItemStack const &, bool);
  MCAPI virtual bool getInteraction(class Player &, class ActorInteraction &, class Vec3 const &);
  MCAPI virtual bool canDestroyBlock(class Block const &) const;
  MCAPI virtual void setAuxValue(int);
  MCAPI virtual void setSize(float, float);
  MCAPI virtual int getLifeSpan(void) const;
  MCAPI virtual void onOrphan(void);
  MCAPI virtual void wobble(void);
  MCAPI virtual bool wasHurt(void);
  MCAPI virtual void startSpinAttack(void);
  MCAPI virtual void stopSpinAttack(void);
  MCAPI virtual void setDamageNearbyMobs(bool);
  MCAPI virtual void renderDebugServerState(class Options const &);
  MCAPI virtual void reloadLootTable(void);
  MCAPI virtual void reloadLootTable(struct EquipmentTableDescription const *);
  MCAPI virtual float getDeletionDelayTimeSeconds(void) const;
  MCAPI virtual void kill(void);
  MCAPI virtual void die(class ActorDamageSource const &);
  MCAPI virtual bool shouldTick(void) const;
  MCAPI virtual void updateEntitySpecificMolangVariables(class RenderParams &);
  MCAPI virtual bool canMakeStepSound(void) const;
  MCAPI virtual void outOfWorld(void);
  MCAPI virtual bool _hurt(class ActorDamageSource const &, int, bool, bool);
  MCAPI virtual void markHurt(void);
  MCAPI virtual void readAdditionalSaveData(class CompoundTag const &, class DataLoadHelper &);
  MCAPI virtual void addAdditionalSaveData(class CompoundTag &);
  MCAPI virtual void _playStepSound(class BlockPos const &, class Block const &);
  MCAPI virtual void _playFlySound(class BlockPos const &, class Block const &);
  MCAPI virtual bool _makeFlySound(void) const;
  MCAPI virtual void checkInsideBlocks(float);
  MCAPI virtual void pushOutOfBlocks(class Vec3 const &);
  MCAPI virtual bool updateWaterState(void);
  MCAPI virtual void doWaterSplashEffect(void);
  MCAPI virtual void spawnTrailBubbles(void);
  MCAPI virtual void updateInsideBlock(void);
  MCAPI virtual class LootTable *getLootTable(void);
  MCAPI virtual class LootTable *getDefaultLootTable(void);
  MCAPI virtual void _removeRider(struct ActorUniqueID const &, bool, bool);
  MCAPI virtual void _onSizeUpdated(void);
  MCAPI virtual void _doAutoAttackOnTouch(class Actor &);

  MCAPI bool isRiding(void) const;
  MCAPI bool isFireImmune(void) const;
  MCAPI bool isRider(class Actor const &) const;
  MCAPI bool isUnderWaterfall(void) const;
  MCAPI bool isInsideBorderBlock(float) const;
  MCAPI bool isControlledByLocalInstance(void) const;
  MCAPI bool isWithinRestriction(class BlockPos const &) const;

  MCAPI int getSkinID(void) const;
  MCAPI int getVariant(void) const;
  MCAPI int getStrength(void) const;
  MCAPI int getMarkVariant(void) const;
  MCAPI int getControllingSeat(void);
  MCAPI int getRiderIndex(class Actor &) const;
  MCAPI int getStructuralIntegrity(void) const;
  MCAPI class Vec3 getViewVector(float) const;
  MCAPI enum PaletteColor getColor(void) const;
  MCAPI enum PaletteColor getColor2(void) const;
  MCAPI struct ActorUniqueID const getOwnerId(void) const;
  MCAPI bool getStatusFlag(enum ActorFlags) const;
  MCAPI std::vector<struct ActorLink> getLinks(void) const;
  MCAPI std::unique_ptr<class CompoundTag> getPersistingTradeOffers(void);
  MCAPI class Actor *getTarget(void) const;
  MCAPI class Mob *getOwner(void) const;
  MCAPI class Actor *getRideRoot(void) const;
  MCAPI class Player *getTradingPlayer(void) const;
  MCAPI class Player *getPlayerOwner(void) const;
  MCAPI class ITickingArea *getTickingArea(void);
  MCAPI class MerchantRecipeList *getTradeOffers(void);
  MCAPI class MobEffectInstance const *getEffect(class MobEffect const &) const;
  MCAPI struct ActorUniqueID const &getUniqueID(void) const;

  MCAPI void setSkinID(int);
  MCAPI void setCanFly(bool);
  MCAPI void setHurtDir(int);
  MCAPI void setVariant(int);
  MCAPI void setHurtTime(int);
  MCAPI void setStrength(int);
  MCAPI void setCanClimb(bool);
  MCAPI void setPersistent(void);
  MCAPI void setInvisible(bool);
  MCAPI void setInLove(class Actor *);
  MCAPI void setLimitedLife(int);
  MCAPI void setMarkVariant(int);
  MCAPI void setStrengthMax(int);
  MCAPI void setJumpDuration(int);
  MCAPI void setStructuralIntegrity(int);
  MCAPI void setColor(enum PaletteColor);
  MCAPI void setColor2(enum PaletteColor);
  MCAPI void setTradingPlayer(class Player *);
  MCAPI void setLeashHolder(struct ActorUniqueID);
  MCAPI bool setStatusFlag(enum ActorFlags, bool);
  MCAPI void setActorRendererId(class HashedString);
  MCAPI void setPreviousPosRot(class Vec3 const &, class Vec2 const &);
  MCAPI void setSeatDescription(class Vec3 const &, struct SeatDescription const &);
  MCAPI void setBaseDefinition(struct ActorDefinitionIdentifier const &, bool, bool);

  MCAPI bool hasPlayerRider(void) const;
  MCAPI bool hasRestriction(void) const;
  MCAPI bool hasSubBBInLava(void) const;
  MCAPI bool hasTickingArea(void) const;
  MCAPI bool hasEffect(class MobEffect const &) const;
  MCAPI bool hasFamily(class Util::HashString const &) const;
  MCAPI bool hasTag(std::string const &) const;

  MCAPI void addEffect(class MobEffectInstance const &);
  MCAPI bool addTag(std::string const &);

  MCAPI void removeEffect(int);
  MCAPI void removeAllEffects(void);
  MCAPI void removeAllRiders(bool, bool);
  MCAPI void removeEffectParticles(void);

  MCAPI void updateDescription(void);
  MCAPI void updateTickingData(void);
  MCAPI void updateBBFromDescription(void);
  MCAPI void updateInvisibilityStatus(void);
  MCAPI void updateInBubbleColumnState(void);

  MCAPI void moveTo(class Vec3 const &, class Vec2 const &);
  MCAPI void teleportRidersTo(class Vec3 const &, int, int);

  MCAPI void saveEntityFlags(class CompoundTag &);
  MCAPI std::unique_ptr<class ListTag> saveLinks(void) const;
  MCAPI void savePersistingTrade(std::unique_ptr<class CompoundTag>, int);

  MCAPI bool canCurrentlySwim(void) const;
  MCAPI bool canBeginOrContinueClimbingLadder(bool) const;

  MCAPI void spawnDeathParticles(void);
  MCAPI void spawnDustParticles(int);
  MCAPI void spawnTamingParticles(bool);
  MCAPI void spawnBalloonPopParticles(void);
  MCAPI void spawnEatParticles(class ItemStack const &, int);

  MCAPI void sendMotionToServer(void);
  MCAPI void sendActorDefinitionEventTriggered(std::string const &);
  MCAPI void _sendDirtyActorData(void);

  MCAPI void forEachLeashedActor(class A6AXPEAVfunction);

  MCAPI bool shouldOrphan(class BlockSource &);

  MCAPI bool onLadder(bool) const;
  MCAPI void reload(void);
  MCAPI bool moveChunks(void);
  MCAPI void dropLeash(bool, bool);
  MCAPI void moveBBs(class Vec3 const &);
  MCAPI void positionAllRiders(void);
  MCAPI bool pullInEntity(class Actor &);
  MCAPI bool _tryPlaceAt(class Vec3 const &);
  MCAPI bool tick(class BlockSource &);
  MCAPI class Vec3 _randomHeartPos(void);
  MCAPI void pickUpItem(class ItemActor &, int);
  MCAPI int calculateAttackDamage(class Actor &);
  MCAPI bool hurt(class ActorDamageSource const &, int, bool, bool);
  MCAPI void dropTowards(class ItemStack const &, class Vec3);
  MCAPI void transferTickingArea(class Dimension &);
  MCAPI void initEntity(class EntityRegistryOwned &);
  MCAPI void playSound(enum LevelSoundEvent, class Vec3 const &, int);
  MCAPI void pushBackActionEventToActionQueue(class ActionEvent);
  MCAPI void loadEntityFlags(class CompoundTag const &, class DataLoadHelper &);
  MCAPI void testForCollidableMobs(class BlockSource &, class AABB const &, std::vector<class AABB> &);
  MCAPI std::vector<struct DistanceSortedActor> fetchNearbyActorsSorted(class Vec3 const &, enum ActorType);

  template <typename T> MCAPI T *tryGetComponent(void);
  template <typename T> MCAPI T const *tryGetComponent(void) const;

  inline std::string getEntityName() const {
    return CallServerFunction<std::string>(
        "?getEntityName@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVActor@@@Z", this);
  }

  void teleport(Vec3 const &target, Vec3 const &old, AutomaticID<Dimension, int> dim) {
    CallServerClassMethod<void>(
        "?teleport@TeleportCommand@@AEBAXAEAVActor@@VVec3@@PEAV3@V?$AutomaticID@VDimension@@H@@@Z", this, this, target,
        &old, dim);
  }

  BUILD_ACCESS(class BlockSource *, region, 800);
  BASEAPI ActorRuntimeID getRuntimeID() const;
  BASEAPI SimpleContainer &getEquipmentContainer();
  BASEAPI SimpleContainer &getHandContainer();
  BASEAPI SynchedActorData &getEntityData() const;
};

class Mob : public Actor {
public:
  inline void sendInventory(bool a0) const {
    return CallServerClassMethod<void>("?sendInventory@Mob@@UEAAX_N@Z", this, a0);
  }
};