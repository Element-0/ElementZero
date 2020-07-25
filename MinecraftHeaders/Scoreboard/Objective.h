#pragma once

#include <functional>
#include <unordered_map>
#include <string>

#include "ScoreboardId.h"
#include "ObjectiveCriteria.h"

class Objective {
public:
  std::unordered_map<ScoreboardId, int> smap;
  std::string name, name2;
  ObjectiveCriteria *Criteria;
  MCAPI struct ScoreInfo getPlayerScore(struct ScoreboardId const &) const;
};