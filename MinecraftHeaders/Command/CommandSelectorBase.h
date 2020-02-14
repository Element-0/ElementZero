#pragma once

#include <functional>
#include <vector>
#include <string>
#include <cstdint>
#include "../Core/Vec3.h"
#include "../Core/BlockPos.h"
#include "CommandPosition.h"
#include "InvertableFilter.h"
#include "../Actor/ActorDefinitionIdentifier.h"

class CommandOrigin;
class Actor;
class ActorDefinitionIdentifier;

class CommandSelectorBase {
  uint32_t version;                                                                     // 0
  uint32_t type;                                                                        // 4
  uint32_t order;                                                                       // 8
  std::vector<InvertableFilter<std::string>> namefilters;                               // 16
  std::vector<InvertableFilter<ActorDefinitionIdentifier>> typefilter2;                 // 40
  std::vector<InvertableFilter<std::string>> tagfilters;                                // 64
  std::vector<std::function<bool(CommandOrigin const &, Actor const &)>> customfilters; // 88
  CommandPosition position;                                                             // 112
  BlockPos box;                                                                         // 128
  float radiusMin;                                                                      // 140
  float radiusMax;                                                                      // 144 = 0x7f7fffff (float max)
  uint64_t resultCount;                                                                 // 152 = 0xFFFFFFFF
  bool includeDeadPlayers;                                                              // 160
  bool flag161;                                                                         // 161
  bool flag162;                                                                         // 162
  bool flag163;                                                                         // 163
  bool playerOnly;                                                                      // 164
  bool explicitIdSelector;                                                              // 165

public:
  inline bool isExplicitIdSelector() const { return explicitIdSelector; }
  inline void addNameFilter(InvertableFilter<std::string> const &filter) { namefilters.emplace_back(filter); }
  inline void addTagFilter(InvertableFilter<std::string> const &filter) {
    if (isExplicitIdSelector()) explicitIdSelector = false;
    tagfilters.emplace_back(filter);
  }
  inline void setIncludeDeadPlayers(bool value) { includeDeadPlayers = value; }
  inline void setResultCount(uint64_t value) { resultCount = value; }

  __declspec(dllimport) void addFilter(std::function<bool(CommandOrigin const &, Actor const &)>);
  __declspec(dllimport) void addTypeFilter(InvertableFilter<std::string> const &);
  __declspec(dllimport) CommandSelectorBase();
  __declspec(dllimport) void setBox(BlockPos);
  __declspec(dllimport) void setPosition(CommandPosition);
  __declspec(dllimport) void setRadiusMin(float);
  __declspec(dllimport) void setRadiusMax(float);
  __declspec(dllimport) bool compile(CommandOrigin const &, std::string &);

protected:
  __declspec(dllimport) CommandSelectorBase(bool isPlayer);
};