#include <dllentry.h>
#include <list>
#include <memory>

#include <log.h>
#include <utility>
#include <yaml.h>
#include <base.h>
#include <remote.h>

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

void PostInit() {
  LOGV("Load builtin extension for player list");
  InitPlayerlistHook();
  for (auto [name, fn] : inits()) {
    auto handle = GetLoadedMod(name);
    if (handle) {
      LOGV("Load builtin extension for %s") % name;
      fn();
    } else {
      LOGV("Skip builtin extension for %s: Target mod not loaded") % name;
    }
  }
}

void WorldInit(std::filesystem::path const &) {
  LOGV("connecting to %s") % settings.endpoint;
  state->srv.Connect(settings.endpoint, {settings.name, "element-zero", Common::getServerVersionString()});
  LOGI("Connected to hub");
}

namespace Mod {

struct RemoteImpl : Remote {
  void AddMethod(std::string const &name, WsGw::Handler handler) override {
    if (!state) {
      LOGW("Skip register method: the Remote module not enabled.");
      return;
    }
    LOGV("Register method %s") % name;
    state->srv.RegisterHandler(name, handler);
  }

  void Broadcast(std::string_view name, WsGw::BufferView payload) override {
    if (!state) return;
    state->srv.Broadcast(name, payload);
  }
};

Remote &Remote::GetInstance() {
  static RemoteImpl impl;
  return impl;
}

} // namespace Mod