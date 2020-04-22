#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <playerdb.h>
#include <scriptapi.h>

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
  uint32_t color   = 0; // Not implmented in client
};

class Instance;

class Handle {
  std::weak_ptr<Instance> ref;

public:
  inline bool Valid() { return !ref.expired(); }

  BOSSBARAPI void UpdateText(std::string const &text);

  BOSSBARAPI void UpdatePercent(float percent);

  BOSSBARAPI void UpdateColor(uint32_t color);

  BOSSBARAPI void Hide();

  BOSSBARAPI void Show();

  BOSSBARAPI void Destory();

  BOSSBARAPI static Handle Create(Config const &cfg);

  BOSSBARAPI static JsValueRef InitProto();

  BOSSBARAPI static Scripting::JsObjectWrapper CreateJsObject(Handle const &orig);
};

BOSSBARAPI std::vector<Handle> GetHandlesForPlayer(Player *);

} // namespace Mod::Bossbar

namespace Mod::Scripting {

inline JsValueRef ToJs(Bossbar::Handle const &handle) { return *Bossbar::Handle::CreateJsObject(handle); }
template <> struct HasToJs<Bossbar::Handle> : std::true_type {};

} // namespace Mod::Scripting