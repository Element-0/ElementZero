#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <playerdb.h>

class Player;

#ifdef BossBarSupport_EXPORTS
#  define BOSSBARAPI __declspec(dllexport)
#else
#  define BOSSBARAPI __declspec(dllimport)
#endif

namespace Mod::Bossbar {

struct Config {
  PlayerEntry entry;
  std::string text = "dummy";
  float percent    = 0.5f;
  uint32_t color   = 1;
};

class Instance;

class Handle {
  std::weak_ptr<Instance> ref;

public:
  inline bool Valid() { return !ref.expired(); }

  BOSSBARAPI void UpdateText(std::string const &text);

  BOSSBARAPI void UpdatePercent(float percent);

  BOSSBARAPI void UpdateColor(uint32_t color);

  static BOSSBARAPI Handle Create(Config const &cfg);
};

BOSSBARAPI std::vector<Handle> GetHandlesForPlayer(Player *);

} // namespace Mod::Bossbar