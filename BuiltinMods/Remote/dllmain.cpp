#include <dllentry.h>
#include <list>
#include <memory>

#include <log.h>
#include <utility>
#include <yaml.h>
#include <base.h>

#include <Core/Common.h>

#include "global.h"

DEF_LOGGER("Remote");

void dllenter() {}
void dllexit() {}

Settings settings;
std::unique_ptr<State> state;

DEFAULT_SETTINGS(settings);

static auto &inits() {
  static std::vector<std::pair<char const *, void (*)()>> ret;
  return ret;
}

void AddInitializer(char const *name, void (*fn)()) {
  LOGV("Register extension for %s") % name;
  inits().emplace_back(name, fn);
}

void PreInit() {
  state = std::make_unique<State>();
  AddInitializer("ChatAPI", InitChatHook);
  AddInitializer("Blacklist", InitBlacklistHook);
}

void WorldInit(std::filesystem::path const &) {
  for (auto [name, fn] : inits()) {
    auto handle = GetLoadedMod(name);
    if (handle) {
      LOGV("Load extension for %s") % name;
      fn();
    } else {
      LOGV("Skip extension for %s") % name;
    }
  }
  LOGV("connecting to %s") % settings.endpoint;
  state->srv.Connect(settings.endpoint, {settings.name, "element-zero", Common::getServerVersionString()});
  LOGI("Connected to hub");
}