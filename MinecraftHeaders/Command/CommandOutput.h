#pragma once

#include <string>
#include <vector>
#include "CommandOutputParameter.h"
#include "../dll.h"

class Actor;

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class CommandOutput {
public:
  BASEAPI void success();
  MCAPI void success(std::string const &, std::vector<CommandOutputParameter> const &params = {});
  MCAPI void error(std::string const &, std::vector<CommandOutputParameter> const &params = {});
  MCAPI void addToResultList(std::string const &, Actor const &);
  template <typename T> void set(char const *name, T value);
};