#pragma once

#include <string>
#include <variant>
#include <unordered_map>

#include "../Core/json.h"
#include "../Core/SemVersion.h"
#include "../Core/NBT.h"
#include "../Core/StringKey.h"
#include "../Math/BlockPos.h"
#include "AdventureSettings.h"
#include "WorldTemplateLevelData.h"
#include "GameRules.h"
#include "Abilities.h"
#include "GameVersion.h"
#include "Tick.h"
#include "GameType.h"
#include "Difficulty.h"
#include "GeneratorType.h"
#include "SpawnSettings.h"

#include <modutils.h>

#include "../dll.h"

struct LevelDataValue : public std::variant<
                            int, bool, float, std::string, GeneratorType, GameType, BlockPos, unsigned int,
                            std::unique_ptr<CompoundTag>> {
  using variant::variant;
};

namespace LevelDataKeys {
extern MCAPI StringKey EDUCATION_OID;
extern MCAPI StringKey GAME_TYPE;
extern MCAPI StringKey GENERATOR;
extern MCAPI StringKey LIMITED_WORLD_DEPTH;
extern MCAPI StringKey LIMITED_WORLD_WIDTH;
extern MCAPI StringKey LOADED_PLAYER_TAG;
extern MCAPI StringKey SEED;
extern MCAPI StringKey SPAWN_POS;
}; // namespace LevelDataKeys

// ref: LevelData::getTagData
class LevelData {
public:
  AdventureSettings mAdventureSettings;
  WorldTemplateLevelData mPremiumTemplate;
  GameRules mGameRules;
  Abilities mAbilities;
  std::string mName;
  int mStorageVersion;
  GameVersion mGameVersion;
  int mNetworkVersion;
  SemVersion mInventoryVersion;
  Tick mTick;
  bool mHasSpawnPos;
  BlockPos mLimitedWorldOrigin;
  int mDayCycleTime;
  uint64_t mLastPlayedTime;
  unsigned mServerChunkTickRange = 4;
  float mRainLevel;
  unsigned mRainTime;
  float mLightningLevel;
  unsigned mLightningTime;
  unsigned mNetherScale;
  GameVersion mLastOpenedWithVersion;
  GameType mGameType;
  bool mForceGameType;
  bool mSpawnMobs;
  Json::Value mFlatGenerator;
  int mWorldStartCount;
  bool mHasBeenLoadedInCreative;
  int mEducationEditionOffer;
  bool mEducationFeaturesEnabled;
  bool mSingleUseWorld;
  bool mConfirmedPlatformLockedContent;
  bool mMultiplayerGameIntent;
  bool mMultiplayerGameMode;
  bool mLanGameIntent;
  bool mLanGameMode;
  int mXBLBroadcastIntent;
  int mXBLBroadcastMode;
  int mPlatformBroadcastIntent;
  int mPlatformBroadcastMode;
  bool mCommandsEnabled;
  bool mTexturepacksRequired;
  bool mLockedBehaviorPack;
  bool mLockedResourcePack;
  bool mFromLockedTemplate;
  std::string mEducationProductId;
  bool mUseMsaGamertagsOnly;
  bool mBonusChestEnabled;
  bool mBonusChestSpawned;
  bool mStartWithMapEnabled;
  bool mCenterMapsToOrigin;
  bool mRequiresCopiedPackRemovalCheck;
  bool mOnlySpawnV1Villagers;
  char mNetherType;
  SpawnSettings mSpawnSettings;
  std::unordered_map<StringKey, LevelDataValue> mKV, mAltKV;
  std::string mBiomeOverride;

private:
  template <typename T> T const *_extractValue(StringKey const &) const;

public:
  inline LevelDataValue const *getValue(StringKey const &key) const {
    if (auto kv = mKV.find(key); kv != mKV.end()) return &kv->second;
    if (auto kv = mAltKV.find(key); kv != mAltKV.end()) return &kv->second;
    return nullptr;
  }
  template <typename T> inline T const *extractValue(StringKey const &key) const {
    auto val = getValue(key);
    return std::get_if<T>(val);
  }
  template <typename T> inline void setValue(StringKey const &key, T value) { mKV[key] = LevelDataValue(value); }

  inline BlockPos getSpawnPos() const {
    if (auto value = extractValue<BlockPos>(LevelDataKeys::SPAWN_POS))
      return *value;
    else
      return BlockPos::ZERO;
  }
  inline void setSpawnPos(BlockPos const &pos) { setValue(LevelDataKeys::SPAWN_POS, pos); }

  inline GameType getGameType() const {
    if (auto value = extractValue<::GameType>(LevelDataKeys::GAME_TYPE))
      return *value;
    else
      return GameType::Survival;
  }
  inline void setGameType(GameType type) { setValue(LevelDataKeys::GAME_TYPE, type); }

  inline unsigned getSeed() const {
    if (auto value = extractValue<unsigned>(LevelDataKeys::SEED))
      return *value;
    else
      return 0;
  }
  inline void setSeed(unsigned seed) { setValue(LevelDataKeys::SEED, seed); }

  inline GeneratorType getWorldGenerator() const {
    if (auto value = extractValue<GeneratorType>(LevelDataKeys::GENERATOR))
      return *value;
    else
      return GeneratorType::Normal;
  }
  inline void setWorldGenerator(GeneratorType seed) { setValue(LevelDataKeys::SEED, seed); }

  inline int getLimitedWorldWidth() const {
    if (auto value = extractValue<int>(LevelDataKeys::LIMITED_WORLD_WIDTH))
      return *value;
    else
      return 0;
  }
  inline void setLimitedWorldWidth(int value) { setValue(LevelDataKeys::LIMITED_WORLD_WIDTH, value); }

  inline int getLimitedWorldDepth() const {
    if (auto value = extractValue<int>(LevelDataKeys::LIMITED_WORLD_DEPTH))
      return *value;
    else
      return 0;
  }
  inline void setLimitedWorldDepth(int value) { setValue(LevelDataKeys::LIMITED_WORLD_DEPTH, value); }

  int getSpawnDimension() const { return mSpawnSettings.dim; }
  void setSpawnDimension(int v) { mSpawnSettings.dim = v; }

  DEF_FIELD_RW(unsigned, Seed);
  DEF_FIELD_RW(GeneratorType, WorldGenerator);
  DEF_FIELD_RW(int, LimitedWorldWidth);
  DEF_FIELD_RW(int, LimitedWorldDepth);
  DEF_FIELD_RW(BlockPos, SpawnPos);
  DEF_FIELD_RW(GameType, GameType);
  DEF_FIELD_RW(int, SpawnDimension);
};

static_assert(offsetof(LevelData, mGameRules) == 288);
static_assert(offsetof(LevelData, mAbilities) == 312);
static_assert(offsetof(LevelData, mName) == 632);
static_assert(offsetof(LevelData, mGameVersion) == 672);
static_assert(offsetof(LevelData, mNetworkVersion) == 728);
static_assert(offsetof(LevelData, mInventoryVersion) == 736);
static_assert(offsetof(LevelData, mTick) == 848);
static_assert(offsetof(LevelData, mLimitedWorldOrigin) == 860);
static_assert(offsetof(LevelData, mDayCycleTime) == 872);
static_assert(offsetof(LevelData, mServerChunkTickRange) == 888);
static_assert(offsetof(LevelData, mSingleUseWorld) == 1005);
static_assert(offsetof(LevelData, mEducationProductId) == 1040);
static_assert(offsetof(LevelData, mNetherType) == 1079);
static_assert(offsetof(LevelData, mSpawnSettings) == 1080);
static_assert(offsetof(LevelData, mKV) == 1128);