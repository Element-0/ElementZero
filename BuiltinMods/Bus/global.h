#pragma once
#include <memory>
#include <yaml.h>

#include <mini_bus.hpp>

#include <mods/Bus/api.h>

struct Settings {
  std::string name    = "element-zero";
  std::string host    = "127.0.0.1";
  unsigned short port = 4040;

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.name, node["name"]) && f(self.host, node["host"]) && f(self.port, node["port"]);
  }
};

inline Settings settings;
inline std::unique_ptr<mini_bus::MiniBusClient> client;

struct RegisterAPI {
  static std::map<std::string, void (*)()> &GetMap();
  static std::list<std::pair<std::string, void (*)()>> &GetPreloadList();

  inline RegisterAPI(char const *name, bool check, void (*t)()) {
    if (check)
      GetPreloadList().emplace_back(name, t);
    else
      GetMap().emplace(name, t);
  }
};
