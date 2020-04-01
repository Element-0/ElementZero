#include <dllentry.h>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <thread>

#include <log.h>
#include <yaml.h>
#include <ws-gw.h>

#include <Core/Common.h>

DEF_LOGGER("Remote");

void dllenter() {}
void dllexit() {}

struct Settings {
  std::string name     = "element-zero";
  std::string endpoint = "ws://127.0.0.1:8818/";

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.name, node["name"]) && f(self.endpoint, node["endpoint"]);
  }
} settings;

struct State {
  WsGw::Service srv{[](auto &inp) -> WsGw::Buffer { throw std::runtime_error{"Not implemented"}; }};
};

std::unique_ptr<State> state;

void PreInit() { state = std::make_unique<State>(); }
void WorldInit(std::filesystem::path const &) {
  LOGV("connecting to %s") % settings.endpoint;
  state->srv.Connect(settings.endpoint, {settings.name, "element-zero", Common::getServerVersionString()});
  LOGI("Connected to hub");
}