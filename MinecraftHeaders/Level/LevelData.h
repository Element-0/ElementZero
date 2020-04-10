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
  GameVersion mGameVersion; // !!
  int mNetworkVersion;
  SemVersion mInventoryVersion;
  Tick mTick;
  unsigned mSeed;
  BlockPos mSpawnPos;
  bool mHasSpawnPos;
  BlockPos mWorldCenter;
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
  bool mRequiresCopiedPackRemovalCheck;
  bool mOnlySpawnV1Villagers;
};

static_assert(offsetof(LevelData, mSpawnPos) == 860);