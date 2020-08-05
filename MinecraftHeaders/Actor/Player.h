#pragma once

#include <vector>

#include <hook.h>

#include "Actor.h"
#include "Mob.h"
#include "../Core/ExtendedCertificate.h"
#include "../Container/PlayerInventory.h"
#include "../Container/EnderChestContainer.h"
#include "../Math/Vec3.h"
#include "../Math/BlockPos.h"
#include "../Core/AutomaticID.h"
#include "../Core/VanillaDimensions.h"
#include "../Command/CommandPermissionLevel.h"
#include "../dll.h"

class Packet;
class ServerPlayer;
class NetworkIdentifier;
class Dimension;
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
  class PlayerSpawnPoint {
  public:
    inline static BlockPos InvalidSpawnPos = BlockPos::MIN;
    BlockPos spawn_block = InvalidSpawnPos, spawn_pos = InvalidSpawnPos;
    AutomaticID<class Dimension, int> dim = VanillaDimensions::Undefined;

    inline PlayerSpawnPoint()        = default;
    inline PlayerSpawnPoint &operator=(PlayerSpawnPoint &&) = default;
    inline bool hasSpawnPoint() const { return spawn_block != BlockPos::MIN && dim != VanillaDimensions::Undefined; }
    inline void invalidate() { *this = {}; }
    inline bool isValid() const { return dim != VanillaDimensions::Undefined; }
    inline void setSpawnPoint(BlockPos spawn_block, AutomaticID<class Dimension, int> dim, BlockPos spawn_pos) {
      this->spawn_block = spawn_block;
      this->spawn_pos   = spawn_pos;
      this->dim         = dim;
    }
    inline void setSpawnPoint(BlockPos spawn_block, AutomaticID<class Dimension, int> dim) {
      setSpawnPoint(spawn_block, dim, spawn_block);
    }
  };

  inline ServerPlayer *asServerPlayer() const noexcept {
    return const_cast<ServerPlayer *>(reinterpret_cast<ServerPlayer const *>(this));
  }

  virtual void prepareRegion(class ChunkSource &);
  virtual void destroyRegion(void);
  virtual void suspendRegion(void);
  virtual void _fireWillChangeDimension(void);
  virtual void _fireDimensionChanged(void);
  virtual void changeDimensionWithCredits(class AutomaticID<class Dimension, int>);
  virtual int tickWorld(struct Tick const &);
  virtual void frameUpdate(class FrameUpdateContextBase &);
  virtual std::vector<class ChunkPos> const &getTickingOffsets(void) const;
  virtual void moveView(void);
  virtual void setName(std::string const &);
  virtual void checkMovementStats(class Vec3 const &);
  virtual class StructureFeature *getCurrentStructureFeature() const;
  virtual bool isAutoJumpEnabled() const;
  virtual void respawn(void);
  virtual void resetRot();
  virtual void resetPos(bool);
  virtual bool isInTrialMode();
  virtual bool hasResource(int);
  virtual void completeUsingItem(void);
  virtual void setPermissions(enum CommandPermissionLevel);
  virtual void startDestroying(void);
  virtual void stopDestroying(void);
  virtual void openPortfolio();
  virtual void openBook(int, bool, int, BlockActor *);
  virtual void openTrading(struct ActorUniqueID const &, bool);
  virtual bool canOpenContainerScreen(void);
  virtual void openChalkboard(class ChalkboardBlockActor *, bool);
  virtual void openNpcInteractScreen(Actor &);
  virtual void openInventory(void);
  virtual void displayChatMessage(std::string const &, std::string const &);
  virtual void displayClientMessage(std::string const &);
  virtual void displayTextObjectMessage(class TextObjectRoot const &);
  virtual void displayTextObjectWhisperMessage(class TextObjectRoot const &, std::string const &, std::string const &);
  virtual void
  displayWhisperMessage(std::string const &, std::string const &, std::string const &, std::string const &);
  virtual enum BedSleepingResult startSleepInBed(class BlockPos const &);
  virtual void stopSleepInBed(bool, bool);
  virtual bool canStartSleepInBed(void);
  virtual int getSleepTimer(void) const;
  virtual int getPreviousTickSleepTimer(void) const;
  virtual void openSign(BlockPos const &);
  virtual void playEmote(std::string const &);
  virtual bool isLocalPlayer() const;
  virtual bool isHostingPlayer(void) const;
  virtual bool isLoading(void) const;
  virtual bool isPlayerInitialized(void) const;
  virtual void stopLoading();
  virtual void registerTrackedBoss(struct ActorUniqueID);
  virtual void unRegisterTrackedBoss(struct ActorUniqueID);
  virtual void setPlayerGameType(enum GameType);
  virtual void _crit(class Actor &);
  virtual class IMinecraftEventing *getEventing(void) const;
  virtual unsigned char getUserId();
  virtual void sendEventPacket(class EventPacket &) const;
  virtual void addExperience(int);
  virtual void addLevels(int);
  virtual void setContainerData(class IContainerManager &, int, int);
  virtual void slotChanged(
      class IContainerManager &, class Container &, int, class ItemStack const &, class ItemStack const &, bool);
  virtual void inventoryChanged(class Container &, int, class ItemStack const &, class ItemStack const &, bool);
  virtual void refreshContainer(class IContainerManager &);
  virtual void deleteContainerManager(void);
  virtual void setFieldOfViewModifier(float);
  virtual bool isPositionRelevant(class AutomaticID<class Dimension, int>, class BlockPos const &);
  virtual bool isEntityRelevant(class Actor const &);
  virtual bool isTeacher(void) const;
  virtual void onSuspension(void);
  virtual void onLinkedSlotsChanged(void);
  virtual void startCooldown(class Item const *);
  virtual int getItemCooldownLeft(enum CooldownType) const;
  virtual bool isItemInCooldown(enum CooldownType) const;
  virtual void sendInventoryTransaction(class InventoryTransaction const &) const;
  virtual void sendComplexInventoryTransaction(std::unique_ptr<class ComplexInventoryTransaction>) const;
  virtual void sendNetworkPacket(class Packet &) const;
  virtual class PlayerEventCoordinator &getPlayerEventCoordinator(void);
  virtual class MoveInputHandler *getMoveInputHandler();
  virtual enum InputMode getInputMode(void) const;
  virtual enum ClientPlayMode getPlayMode(void) const;
  virtual void reportMovementTelemetry(enum MovementEventType);
  virtual void onMovePlayerPacketNormal(class Vec3 const &, class Vec2 const &, float);

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

  BUILD_ACCESS_COMPAT(PlayerInventory &, Inventory);
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
