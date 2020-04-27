#pragma once

#include <string>

#include "../Core/json.h"
#include "../Core/SemVersion.h"
#include "../Core/NBT.h"
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
  unsigned mSeed;
  BlockPos mSpawnPos;
  bool mHasSpawnPos;
  BlockPos mLimitedWorldOrigin;
  int mDayCycleTime;
  uint64_t mLastPlayedTime;
  CompoundTag mDefaultPlayerTag;
  unsigned mServerTickRange;
  float mRainLevel;
  unsigned mRainTime;
  float mLightningLevel;
  unsigned mLightningTime;
  unsigned mNetherScale;
  GameVersion mLastOpenedWithVersion;
  GameType mGameType;
  Difficulty mDifficulty;
  bool mForceGameType;
  bool mSpawnMobs;
  GeneratorType mGeneratorType;
  Json::Value mFlatGenerator;
  int mWorldStartCount;
  bool mHasBeenLoadedInCreative;
  int mEducationEditionOffer;
  bool mEducationFeaturesEnabled;
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
  std::string mEducationOid;
  std::string mEducationProductId;
  bool mUseMsaGamertagsOnly;
  bool mBonusChestEnabled;
  bool mBonusChestSpawned;
  bool mStartWithMapEnabled;
  bool mCenterMapsToOrigin;
  bool mRequiresCopiedPackRemovalCheck;
  bool mOnlySpawnV1Villagers;
};

static_assert(offsetof(LevelData, mName) == 632);
static_assert(offsetof(LevelData, mGameVersion) == 672);
static_assert(offsetof(LevelData, mNetworkVersion) == 728);
static_assert(offsetof(LevelData, mTick) == 848);
static_assert(offsetof(LevelData, mSpawnPos) == 860);
static_assert(offsetof(LevelData, mLastPlayedTime) == 896);
static_assert(offsetof(LevelData, mDefaultPlayerTag) == 904);
static_assert(offsetof(LevelData, mServerTickRange) == 928);
static_assert(offsetof(LevelData, mRainTime) == 936);
static_assert(offsetof(LevelData, mNetherScale) == 948);
static_assert(offsetof(LevelData, mLastOpenedWithVersion) == 952);
static_assert(offsetof(LevelData, mGameType) == 1008);
static_assert(offsetof(LevelData, mSpawnMobs) == 1017);
static_assert(offsetof(LevelData, mFlatGenerator) == 1024);
static_assert(offsetof(LevelData, mWorldStartCount) == 1040);
static_assert(offsetof(LevelData, mHasBeenLoadedInCreative) == 1044);
static_assert(offsetof(LevelData, mEducationEditionOffer) == 1048);
static_assert(offsetof(LevelData, mEducationFeaturesEnabled) == 1052);
static_assert(offsetof(LevelData, mXBLBroadcastIntent) == 1060);
static_assert(offsetof(LevelData, mPlatformBroadcastIntent) == 1068);
static_assert(offsetof(LevelData, mCommandsEnabled) == 1076);
static_assert(offsetof(LevelData, mTexturepacksRequired) == 1077);
static_assert(offsetof(LevelData, mEducationOid) == 1088);
static_assert(offsetof(LevelData, mForceGameType) == 1016);
static_assert(offsetof(LevelData, mBonusChestEnabled) == 1153);
static_assert(offsetof(LevelData, mCenterMapsToOrigin) == 1156);
static_assert(offsetof(LevelData, mRequiresCopiedPackRemovalCheck) == 1157);
static_assert(offsetof(LevelData, mOnlySpawnV1Villagers) == 1158);