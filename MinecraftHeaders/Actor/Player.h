#pragma once

#include <vector>

#include <hook.h>

#include "Actor.h"
#include "Mob.h"
#include "../Core/ExtendedCertificate.h"
#include "../Container/PlayerInventoryProxy.h"
#include "../Container/EnderChestContainer.h"
#include "../Math/Vec3.h"
#include "../Math/BlockPos.h"
#include "../Core/AutomaticID.h"
#include "../Command/CommandPermissionLevel.h"
#include "../dll.h"

class Packet;
class ServerPlayer;
class NetworkIdentifier;
enum class StructureFeatureType;
enum class AbilitiesIndex;
enum class BedSleepingResult;
enum class GameType;
enum class CooldownType;
enum class ClientPlayMode;
enum class MovementEventType;
enum class PlayerUISlot;

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class Player : public Mob {
public:
  inline ServerPlayer *asServerPlayer() const noexcept {
    return const_cast<ServerPlayer *>(reinterpret_cast<ServerPlayer const *>(this));
  }

  MCAPI virtual void prepareRegion(class ChunkSource &);
  MCAPI virtual void destroyRegion(void);
  MCAPI virtual void suspendRegion(void);
  MCAPI virtual void _fireWillChangeDimension(void);
  MCAPI virtual void _fireDimensionChanged(void);
  MCAPI virtual void changeDimensionWithCredits(class AutomaticID<class Dimension, int>);
  MCAPI virtual int tickWorld(struct Tick const &);
  MCAPI virtual void frameUpdate(class FrameUpdateContextBase &) = 0;
  MCAPI virtual std::vector<class ChunkPos> const &getTickingOffsets(void) const;
  MCAPI virtual void moveView(void);
  MCAPI virtual void moveSpawnView(class Vec3 const &);
  MCAPI virtual void setName(std::string const &);
  MCAPI virtual void checkMovementStats(class Vec3 const &);
  MCAPI virtual enum StructureFeatureType getCurrentStructureFeature(void) const;
  MCAPI virtual bool isAutoJumpEnabled(void) const;
  MCAPI virtual void respawn(void);
  MCAPI virtual void resetRot(void);
  MCAPI virtual void resetPos(bool);
  MCAPI virtual bool isInTrialMode(void);
  MCAPI virtual bool hasResource(int);
  MCAPI virtual void completeUsingItem(void);
  MCAPI virtual void setPermissions(enum CommandPermissionLevel);
  MCAPI virtual void startCrafting(class BlockPos const &, bool);
  MCAPI virtual void startStonecutting(class BlockPos const &);
  MCAPI virtual void startDestroying(void);
  MCAPI virtual void stopDestroying(void);
  MCAPI virtual void openContainer(class BlockPos const &);
  MCAPI virtual void openContainer(struct ActorUniqueID const &);
  MCAPI virtual void openFurnace(class BlockPos const &);
  MCAPI virtual void openBlastFurnace(class BlockPos const &);
  MCAPI virtual void openSmoker(class BlockPos const &);
  MCAPI virtual void openEnchanter(class BlockPos const &);
  MCAPI virtual void openAnvil(class BlockPos const &);
  MCAPI virtual void openGrindstone(class BlockPos const &);
  MCAPI virtual void openBrewingStand(class BlockPos const &);
  MCAPI virtual void openHopper(class BlockPos const &);
  MCAPI virtual void openHopper(struct ActorUniqueID const &);
  MCAPI virtual void openDispenser(class BlockPos const &, bool);
  MCAPI virtual void openBeacon(class BlockPos const &);
  MCAPI virtual void openPortfolio(void);
  MCAPI virtual void openBook(int, bool, int, class BlockActor *);
  MCAPI virtual void openCommandBlock(class BlockPos const &);
  MCAPI virtual void openCommandBlockMinecart(struct ActorUniqueID const &);
  MCAPI virtual void openHorseInventory(struct ActorUniqueID const &);
  MCAPI virtual void openTrading(struct ActorUniqueID const &, bool);
  MCAPI virtual bool canOpenContainerScreen(void);
  MCAPI virtual void openChalkboard(class ChalkboardBlockActor &, bool);
  MCAPI virtual void openNpcInteractScreen(class Actor &);
  MCAPI virtual void openInventory(void);
  MCAPI virtual void openStructureEditor(class BlockPos const &);
  MCAPI virtual void openLabTable(class BlockPos const &);
  MCAPI virtual void openElementConstructor(class BlockPos const &);
  MCAPI virtual void openCompoundCreator(class BlockPos const &);
  MCAPI virtual void openMaterialReducer(class BlockPos const &);
  MCAPI virtual void openLoom(class BlockPos const &);
  MCAPI virtual void openStonecutter(class BlockPos const &);
  MCAPI virtual void openCartographyTable(class BlockPos const &);
  MCAPI virtual void displayChatMessage(std::string const &, std::string const &);
  MCAPI virtual void displayClientMessage(std::string const &);
  MCAPI virtual void displayLocalizableMessage(std::string const &, class std::vector<std::string> const &);
  MCAPI virtual void displayTextObjectMessage(class TextObjectRoot const &, std::string const &, std::string const &);
  MCAPI virtual void
  displayWhisperMessage(std::string const &, std::string const &, std::string const &, std::string const &);
  MCAPI virtual enum BedSleepingResult startSleepInBed(class BlockPos const &);
  MCAPI virtual void stopSleepInBed(bool, bool);
  MCAPI virtual bool canStartSleepInBed(void);
  MCAPI virtual int getSleepTimer(void) const;
  MCAPI virtual int getPreviousTickSleepTimer(void) const;
  MCAPI virtual void openSign(class BlockPos const &);
  MCAPI virtual void playEmote(std::string const &, bool);
  MCAPI virtual bool isLocalPlayer(void) const;
  MCAPI virtual bool isHostingPlayer(void) const;
  MCAPI virtual bool isLoading(void) const;
  MCAPI virtual bool isPlayerInitialized(void) const;
  MCAPI virtual void stopLoading(void);
  MCAPI virtual void registerTrackedBoss(struct ActorUniqueID);
  MCAPI virtual void unRegisterTrackedBoss(struct ActorUniqueID);
  MCAPI virtual void setPlayerGameType(enum GameType);
  MCAPI virtual void _crit(class Actor &);
  MCAPI virtual class IMinecraftEventing *getEventing(void) const;
  MCAPI virtual unsigned int getUserId(void) const;
  MCAPI virtual void sendEventPacket(class EventPacket &) const;
  MCAPI virtual void addExperience(int);
  MCAPI virtual void addLevels(int);
  MCAPI virtual void setContainerData(class IContainerManager &, int, int) = 0;
  MCAPI virtual void
  slotChanged(class IContainerManager &, int, class ItemStack const &, class ItemStack const &, bool) = 0;
  MCAPI virtual void inventoryChanged(class Container &, int, class ItemStack const &, class ItemStack const &);
  MCAPI virtual void refreshContainer(class IContainerManager &) = 0;
  MCAPI virtual void deleteContainerManager(void);
  MCAPI virtual void setFieldOfViewModifier(float);
  MCAPI virtual bool isPositionRelevant(class AutomaticID<class Dimension, int>, class BlockPos const &);
  MCAPI virtual bool isEntityRelevant(class Actor const &);
  MCAPI virtual bool isTeacher(void) const = 0;
  MCAPI virtual void onSuspension(void);
  MCAPI virtual void onLinkedSlotsChanged(void);
  MCAPI virtual void startCooldown(class Item const *);
  MCAPI virtual int getItemCooldownLeft(enum CooldownType) const;
  MCAPI virtual bool isItemInCooldown(enum CooldownType) const;
  MCAPI virtual void sendInventoryTransaction(class InventoryTransaction const &) const = 0;
  MCAPI virtual void
      sendComplexInventoryTransaction(class std::unique_ptr<class ComplexInventoryTransaction>) const = 0;
  MCAPI virtual void sendNetworkPacket(class Packet &) const;
  MCAPI virtual class PlayerEventCoordinator &getPlayerEventCoordinator(void);
  MCAPI virtual class MoveInputHandler *getMoveInputHandler(void) = 0;
  MCAPI virtual enum InputMode getInputMode(void) const           = 0;
  MCAPI virtual enum ClientPlayMode getPlayMode(void) const       = 0;
  MCAPI virtual void reportMovementTelemetry(enum MovementEventType);
  MCAPI virtual void onMovePlayerPacketNormal(class Vec3 const &, class Vec2 const &, float);

  // SHARED
  MCAPI virtual void feed(int);
  MCAPI virtual void aiStep(void);
  MCAPI virtual void remove(void);
  MCAPI virtual float getSpeed(void) const;
  MCAPI virtual void rideTick(void);
  MCAPI virtual void setSpeed(float);
  MCAPI virtual void travel(float, float, float);
  MCAPI virtual bool useNewAi(void) const;
  MCAPI virtual bool isJumping(void) const;
  MCAPI virtual void normalTick(void);
  MCAPI virtual bool isBlocking(void) const;
  MCAPI virtual bool isCreative(void) const;
  MCAPI virtual bool isImmobile(void) const;
  MCAPI virtual bool isSleeping(void) const;
  MCAPI virtual bool isAdventure(void) const;
  MCAPI virtual bool isShootable(void);
  MCAPI virtual void stopSwimming(void);
  MCAPI virtual bool consumeTotem(void);
  MCAPI virtual void dropEquipment(void);
  MCAPI virtual void setSleeping(bool);
  MCAPI virtual void startSwimming(void);
  MCAPI virtual void updateGliding(void);
  MCAPI virtual void jumpFromGround(void);
  MCAPI virtual void move(class Vec3 const &);
  MCAPI virtual void stopSpinAttack(void);
  MCAPI virtual float getCameraOffset(void) const;
  MCAPI virtual float getRidingHeight(void);
  MCAPI virtual bool isWorldBuilder(void);
  MCAPI virtual void sendInventory(bool);
  MCAPI virtual void startSpinAttack(void);
  MCAPI virtual int getPortalCooldown(void) const;
  MCAPI virtual int getPortalWaitTime(void) const;
  MCAPI virtual bool attack(class Actor &);
  MCAPI virtual int getItemUseDuration(void);
  MCAPI virtual bool add(class ItemStack &);
  MCAPI virtual void onSynchedDataUpdate(int);
  MCAPI virtual bool getAlwaysShowNameTag(void) const;
  MCAPI virtual void adjustDamageAmount(int &) const;
  MCAPI virtual bool drop(class ItemStack const &, bool);
  MCAPI virtual bool isCreativeModeAllowed(void);
  MCAPI virtual bool canAddRider(class Actor &) const;
  MCAPI virtual bool canBePulledIntoVehicle(void) const;
  MCAPI virtual bool canExistWhenDisallowMob(void) const;
  MCAPI virtual float getMapDecorationRotation(void) const;
  MCAPI virtual void sendMotionPacketIfNeeded(void);
  MCAPI virtual void teleportTo(class Vec3 const &, bool, int, int);
  MCAPI virtual float getItemUseStartupProgress(void);
  MCAPI virtual void awardKillScore(class Actor &, int);
  MCAPI virtual void die(class ActorDamageSource const &);
  MCAPI virtual float getItemUseIntervalProgress(void);
  MCAPI virtual class ItemStack const &getCarriedItem(void) const;
  MCAPI virtual enum ActorType getEntityTypeId(void) const;
  MCAPI virtual void setCarriedItem(class ItemStack const &);
  MCAPI virtual void setOffhandSlot(class ItemStack const &);
  MCAPI virtual class ItemStack const &getEquippedTotem(void) const;
  MCAPI virtual void handleEntityEvent(enum ActorEvent, int);
  MCAPI virtual class HashedString const &getActorRendererId(void) const;
  MCAPI virtual void setArmor(enum ArmorSlot, class ItemStack const &);
  MCAPI virtual void dropEquipment(class ActorDamageSource const &, int);
  MCAPI virtual void actuallyHurt(int, class ActorDamageSource const *, bool);
  MCAPI virtual class Vec3 getAttachPos(enum ActorLocation, float) const;
  MCAPI virtual bool isDamageBlocked(class ActorDamageSource const &) const;
  MCAPI virtual bool isInvulnerableTo(class ActorDamageSource const &) const;
  MCAPI virtual void useItem(class ItemStack &, enum ItemUseMethod, bool);
  MCAPI virtual class AnimationComponent &getAnimationComponent(void);
  MCAPI virtual enum CommandPermissionLevel getCommandPermissionLevel(void) const;
  MCAPI virtual void reloadHardcoded(enum Actor::InitializationMethod, class VariantParameterList const &);
  MCAPI virtual class std::unique_ptr<class Packet, struct std::default_delete<class Packet>> getAddPacket(void);
  MCAPI virtual class std::vector<class ItemStack const *, class std::allocator<class ItemStack const *>>
  getAllHand(void) const;
  MCAPI virtual void initializeComponents(enum Actor::InitializationMethod, class VariantParameterList const &);
  MCAPI virtual class std::vector<class ItemStack const *, class std::allocator<class ItemStack const *>>
  getAllEquipment(void) const;
  MCAPI virtual std::string getFormattedNameTag(void) const;

  bool isInRaid(void) const;
  bool isSurvival(void) const;
  bool isInCreativeMode(void) const;
  bool isHiddenFrom(class Mob &) const;

  bool canBeSeenOnMap(void) const;
  bool canUseOperatorBlocks(void) const;
  bool canDestroy(class Block const &) const;
  bool canUseAbility(enum AbilitiesIndex) const;

  class Agent *getAgent(void) const;
  int getDirection(void) const;
  int getXpNeededForNextLevel(void) const;
  float getDestroySpeed(class Block const &) const;
  float getDestroyProgress(class Block const &) const;
  class ItemStack const &getSelectedItem(void) const;
  enum GameType getPlayerGameType(void) const;
  class ItemStack const &getCurrentActiveShield(void) const;
  class EnderChestContainer *getEnderChestContainer(void);

  void updateTeleportDestPos(void);
  void updateInventoryTransactions(void);
  void updateSkin(class SerializedSkin const &, int);

  void setAgent(class Agent *);
  void setRespawnPosition(class BlockPos const &, bool);
  void setBedRespawnPosition(class BlockPos const &);
  void setPlayerUIItem(enum PlayerUISlot, class ItemStack const &);
  void setContainerManager(class std::shared_ptr<class IContainerManager>);

  void eat(int, float);
  void startUsingItem(class ItemStack const &, int);
  void stopUsingItem(void);
  void releaseUsingItem(void);
  void stopGliding(void);
  void resetPlayerLevel(void);
  void handleJumpEffects(void);
  bool take(class Actor &, int, int);
  void updateTrackedBosses(void);
  void causeFoodExhaustion(float);
  bool checkNeedAutoJump(float, float);
  void clearRespawnPosition(void);
  void recheckSpawnPosition(void);

  BUILD_ACCESS_COMPAT(PlayerInventoryProxy &, Inventory);
  BUILD_ACCESS_COMPAT(class Certificate &, Certificate);
  BUILD_ACCESS_COMPAT(class NetworkIdentifier const &, NetworkIdentifier);
  BUILD_ACCESS_COMPAT(class BlockPos &, SpawnPosition);
  BUILD_ACCESS_COMPAT(std::string &, DeviceId);
  BUILD_ACCESS_COMPAT(std::string &, ClientPlatformId);
  BUILD_ACCESS_COMPAT(std::string &, PlatformOfflineId);
  BUILD_ACCESS_COMPAT(std::string &, ClientPlatformOnlineId);
  BUILD_ACCESS_COMPAT(unsigned char, ClientSubId);
  BASEAPI void kick();
};
