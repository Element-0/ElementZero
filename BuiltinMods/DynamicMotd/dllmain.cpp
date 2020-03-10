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

auto GetNextMotd() {
  static auto it = settings.List.begin();
  auto nit       = it;
  ++it;
  if (it == settings.List.end()) it = settings.List.begin();
  return nit;
}

void handler(Mod::Scheduler::Token) {
  auto handler       = LocateService<ServerNetworkHandler>();
  handler->GetMotd() = *GetNextMotd();
  handler->updateServerAnnouncement();
}

void WorldInit(std::filesystem::path const &) {
  using namespace Mod::Scheduler;
  DEF_LOGGER("DynamicMotd");
  if (settings.List.empty()) {
    LOGW("Failed to load dynamic motd list");
    return;
  }
  LOGW("Starting dynamic motd, count: %d") % settings.List.size();
  Mod::Scheduler::SetInterval(std::chrono::seconds(settings.Interval), handler);
}

void dllenter(){};
void dllexit(){};