#pragma once

#include "CommandSelectorBase.h"
#include "CommandSelectorResults.h"
#include <type_traits>

class Player;
class Actor;
class CommandOrigin;

template <typename T> class CommandSelector : public CommandSelectorBase {
public:
  inline CommandSelector() : CommandSelectorBase(std::is_same_v<T, Player>) {}
  __declspec(dllimport) CommandSelectorResults<T> results(CommandOrigin const &) const;
};