#include <vector>
#include <chrono>

#include <Net/ServerNetworkHandler.h>

#include <dllentry.h>
#include <yaml.h>
#include <log.h>
#include <scheduler.h>

struct Settings {
  std::vector<std::string> List;
  int Interval = 5;

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.List, node["list"]) && f(self.Interval, node["interval"]);
  }
} settings;

DEFAULT_SETTINGS(settings);
DEF_LOGGER("DynamicMotd");

class MotdState {
  std::vector<std::string>::iterator it;
  bool health = true;

public:
  void Init() { it = settings.List.begin(); }

  std::string GetNext() {
    if (!health) return "ERROR";
    auto ret = *it++;
    if (it == settings.List.end()) it = settings.List.begin();
    return ret;
  }

  operator bool() const { return health; }

  void SetHealth(bool health) { this->health = health; }
} state;

static Mod::Scheduler::Token tok;

void handler(Mod::Scheduler::Token) {
  auto handler       = LocateService<ServerNetworkHandler>();
  handler->GetMotd() = state.GetNext();
  handler->updateServerAnnouncement();
}

void WorldInit(std::filesystem::path const &) {
  using namespace Mod::Scheduler;
  if (settings.List.empty()) {
    LOGW("Failed to load dynamic motd list");
    return;
  }
  LOGW("Starting dynamic motd, count: %d") % settings.List.size();
  state.Init();
  tok = Mod::Scheduler::SetInterval(std::chrono::seconds(settings.Interval), handler);
}

void AfterReload() {
  Mod::Scheduler::ClearInterval(tok);
  if (settings.List.empty()) {
    LOGW("Failed to reload dynamic motd list");
    state.SetHealth(false);
    return;
  }
  LOGW("Reload dynamic motd, count: %d") % settings.List.size();
  state.Init();
  state.SetHealth(true);
  tok = Mod::Scheduler::SetInterval(std::chrono::seconds(settings.Interval), handler);
}

void dllenter(){};
void dllexit(){};