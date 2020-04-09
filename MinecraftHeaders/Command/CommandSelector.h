#pragma once

#include <type_traits>

#include "CommandSelectorBase.h"
#include "CommandSelectorResults.h"
#include "../dll.h"

class Player;
class Actor;
class CommandOrigin;

template <typename T> class CommandSelector : public CommandSelectorBase {
public:
  inline CommandSelector() : CommandSelectorBase(std::is_same_v<T, Player>) {}
  MCAPI CommandSelectorResults<T> results(CommandOrigin const &) const;
};