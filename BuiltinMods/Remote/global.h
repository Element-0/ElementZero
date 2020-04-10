#include <string>
#include <exception>
#include <memory>

#include <ws-gw.h>
#include <yaml.h>

struct Settings {
  std::string name     = "element-zero";
  std::string endpoint = "ws://127.0.0.1:8818/";

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.name, node["name"]) && f(self.endpoint, node["endpoint"]);
  }
};

extern Settings settings;

struct State {
  WsGw::Service srv{[](auto &inp, auto cb) { cb(std::make_exception_ptr(std::runtime_error{"Not implemented"}), {}); }};
};

extern std::unique_ptr<State> state;

void InitPlayerlistHook();
void InitChatHook();
void InitBlacklistHook();