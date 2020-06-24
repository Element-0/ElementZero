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
  MCAPI virtual void startDestroying(void);
  MCAPI virtual void stopDestroying(void);
  MCAPI virtual void openPortfolio(void);
  MCAPI virtual void openBook(int, bool, int, class BlockActor *);
  MCAPI virtual void openTrading(struct ActorUniqueID const &, bool);
  MCAPI virtual bool canOpenContainerScreen(void);
  MCAPI virtual void openChalkboard(class ChalkboardBlockActor &, bool);
  MCAPI virtual void openNpcInteractScreen(class Actor &);
  MCAPI virtual void openInventory(void);
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
