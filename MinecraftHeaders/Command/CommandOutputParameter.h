#pragma once

#include <string>
#include <vector>
#include <boost/format.hpp>
#include "../Math/BlockPos.h"
#include "../Math/Vec3.h"
#include "../Actor/Actor.h"
#include "../dll.h"

class Player;

class CommandOutputParameter {
  std::string str;
  int type;

public:
  MCAPI CommandOutputParameter(std::string const &);
  MCAPI CommandOutputParameter(int);
  MCAPI CommandOutputParameter(BlockPos);
  MCAPI CommandOutputParameter(std::vector<Player const *> const &);
  MCAPI CommandOutputParameter(std::vector<std::string> const &);

  inline CommandOutputParameter(std::string str, int type) : str(str), type(type) {}

  inline CommandOutputParameter(Actor const *actor) : str(actor->getEntityName()), type(1) {}
  inline CommandOutputParameter(Actor const &actor) : str(actor.getEntityName()), type(1) {}
  inline CommandOutputParameter(char const *str) : str(str), type(0) {}
  inline CommandOutputParameter(uint64_t val) : str(std::to_string(val)), type(0) {}
  inline CommandOutputParameter(int64_t val) : str(std::to_string(val)), type(0) {}
  inline CommandOutputParameter(float val) : str(std::to_string(val)), type(0) {}
  inline CommandOutputParameter(double val) : str(std::to_string(val)), type(0) {}
  inline CommandOutputParameter(Vec3 val)
      : str((boost::format("%.2f, %.2f, %.2f") % val.x % val.y % val.z).str()), type(0) {}
};