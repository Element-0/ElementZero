#pragma once

#include <cstdint>
#include <string>

#include "../Actor/ActorUniqueID.h"
#include "ScoreboardId.h"
#include "PlayerScoreboardId.h"
#include "IdentityDefinition.h"

struct ScorePacketInfo {
  ScoreboardId sid;
  std::string obj_name;
  uint32_t score;
  IdentityDefinition::Type type;
  PlayerScoreboardId pid;
  ActorUniqueID aid;
  std::string fake_name;

  inline ScorePacketInfo(ScoreboardId const &sid) : sid(sid) {}
};