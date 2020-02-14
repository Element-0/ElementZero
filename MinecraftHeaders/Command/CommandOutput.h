#pragma once

#include <string>
#include <vector>
#include "CommandOutputParameter.h"

class CommandOutput {
public:
  __declspec(dllimport) void success(std::string const &, std::vector<CommandOutputParameter> const &params = {});
  __declspec(dllimport) void error(std::string const &, std::vector<CommandOutputParameter> const &params = {});
  __declspec(dllimport) void addToResultList(std::string const &, Actor const &);
  template <typename T> void set(char const *name, T value);
};