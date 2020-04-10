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

// static auto &inits() {
//   static std::vector<std::pair<char const *, void (*)()>> ret;
//   return ret;
// }

std::map<std::string, void (*)()> &RegisterAPI::GetMap() {
  static std::map<std::string, void (*)()> temp;
  return temp;
}
std::list<std::pair<std::string, void (*)()>> &RegisterAPI::GetPreloadList() {
  static std::list<std::pair<std::string, void (*)()>> temp;
  return temp;
}

RegisterAPI::RegisterAPI(char const *name, bool check, void (*t)()) {
  if (check)
    GetMap().emplace(name, t);
  else
    GetPreloadList().emplace_back(name, t);
}

// void AddInitializer(char const *name, void (*fn)()) {
//   LOGV("Register extension for %s") % name;
//   inits().emplace_back(name, fn);
// }

void PreInit() {
  state = std::make_unique<State>();
  // AddInitializer("ChatAPI", InitChatHook);
  // AddInitializer("Blacklist", InitBlacklistHook);
}

void PostInit() {
  for (auto [name, fn] : RegisterAPI::GetPreloadList()) {
    LOGV("Load builtin extension for %s") % name;
    fn();
  }
  for (auto [name, fn] : RegisterAPI::GetMap()) {
    if (GetLoadedMod(name.c_str())) {
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
  void AddMethod(std::string const &name, WsGw::SyncHandler handler) override {
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