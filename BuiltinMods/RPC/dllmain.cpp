#include <dllentry.h>
#include <exception>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <yaml.h>
#include <ws-gw.h>

void dllenter() {}
void dllexit() {}

struct Settings {
  std::string endpoint = "ws://127.0.0.1:8818";

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.endpoint, node["endpoint"]);
  }
} settings;

WsGw::Service srv{[](auto &inp) -> WsGw::Buffer { throw std::runtime_error{"Not implemented"}; }};
std::exception_ptr ep;

void PostInit() { srv.Connect(settings.endpoint); }