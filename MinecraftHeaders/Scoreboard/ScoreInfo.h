#pragma once
#include <hook.h>
#include<Scoreboard/Objective.h>
#include <Scoreboard/ScoreboardId.h>
struct ScoreInfo {
  Objective *mObjective;
  bool mValid;
  int *mValue;
  auto getcount() { return *(int *) ((unsigned long long) (this) + 12);}
};