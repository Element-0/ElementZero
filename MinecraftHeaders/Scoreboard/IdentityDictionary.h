#pragma once

#include <unordered_map>
#include <string>

#include "../Actor/ActorUniqueID.h"
#include "ScoreboardId.h"
#include "PlayerScoreboardId.h"
#include "../dll.h"

class IdentityDictionary {
public:
  std::unordered_map<PlayerScoreboardId, ScoreboardId> pmap;
  std::unordered_map<ActorUniqueID, ScoreboardId> amap;
  std::unordered_map<std::string, ScoreboardId> fmap;
  std::unordered_map<ScoreboardId, IdentityDefinition> dmap;
};