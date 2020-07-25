#include<cstring>
#include <Scoreboard/ScoreboardId.h>
#include <Scoreboard/ScoreInfo.h>
#include<hook.h>

class Scoreboard {
public:
  MCAPI struct ScoreboardId const &getScoreboardId(class Actor const &) const;
  MCAPI class std::vector<struct ScoreInfo> getIdScores(struct ScoreboardId const &) const;
  MCAPI class Objective *getObjective(std::string const &) const;
};