#pragma once

#include <string>
#include <map>
#include <yaml.h>

static struct Settings {
  bool ModEnabled = true;
  std::map<std::string, YAML::Node> ModSettings;
  struct LogSettingsType {
    std::string Format   = "%1$c [%2$s] (%3$s:%4$d) %5$s";
    std::string Database = "log.db";
  } LogSettings;
  std::string UserDatabase = "user.db";
} settings;

namespace YAML {

template <> struct convert<decltype(std::declval<Settings>().LogSettings)> {
  static bool decode(const Node &node, decltype(std::declval<Settings>().LogSettings) &rhs) {
    yaml_assign(rhs.Format, node["format"]);
    yaml_assign(rhs.Database, node["database"]);
    return true;
  }
};

template <> struct convert<Settings> {
  static bool decode(const Node &node, Settings &rhs) {
    yaml_assign(rhs.ModEnabled, node["mod-enabled"]);
    yaml_assign(rhs.LogSettings, node["log-settings"]);
    yaml_assign(rhs.ModSettings, node["mods"]);
    yaml_assign(rhs.UserDatabase, node["user-database"]);
    return true;
  }
};

} // namespace YAML