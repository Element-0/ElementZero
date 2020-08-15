#pragma once
#include <memory>
#include <yaml.h>

#include <mini_bus.hpp>

#include <mods/Bus.h>

struct Settings {
  std::string name               = "element-zero";
  std::string host               = "127.0.0.1";
  unsigned short port            = 4040;
  unsigned short reconnect_delay = 10;

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.name, node["name"]) && f(self.host, node["host"]) && f(self.port, node["port"]) && f(self.reconnect_delay, node["reconnect-delay"]);
  }
};

inline Settings settings;
inline std::unique_ptr<mini_bus::MiniBusClient> client;

struct RegisterAPI {
  static std::map<std::string, void (*)()> &GetMap();
  static std::list<std::pair<std::string, void (*)()>> &GetPreloadList();

  inline RegisterAPI(char const *name, bool check, void (*t)()) {
    if (check)
      GetMap().emplace(name, t);
    else
      GetPreloadList().emplace_back(name, t);
  }
};
