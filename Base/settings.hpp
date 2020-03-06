#pragma once

#include <string>
#include <map>
#include <vector>
#include <yaml.h>

struct Settings {
  bool ModEnabled = true;
  std::map<std::string, YAML::Node> ModSettings;
  struct LogSettingsType {
    std::string Format   = "%1$c [%2$s] (%3$s:%4$d) %5$s";
    std::string Database = "log.db";
    bool HideVerbose     = false;
    std::vector<std::string> Filters;
    struct DecorationPair {
      std::string Before = "";
      std::string After  = "";

      template <typename IO> static inline bool io(IO f, DecorationPair &pair, YAML::Node &node) {
        return f(pair.Before, node["before"]) && f(pair.After, node["after"]);
      }
    };
    DecorationPair Decorations[5];

    template <typename IO> static inline bool io(IO f, LogSettingsType &log, YAML::Node &node) {
      return f(log.Format, node["format"]) && f(log.Database, node["database"]) &&
             f(log.HideVerbose, node["hide-verbose"]) && f(log.Filters, node["filters"]) &&
             f(log.Decorations[0], node["verbose"]) && f(log.Decorations[1], node["information"]) &&
             f(log.Decorations[2], node["warning"]) && f(log.Decorations[3], node["error"]) &&
             f(log.Decorations[4], node["unknown"]);
    }
  } LogSettings;
  std::string UserDatabase = "user.db";

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.ModEnabled, node["mod-enabled"]) && f(settings.LogSettings, node["log-settings"]) &&
           f(settings.ModSettings, node["mods"]) && f(settings.UserDatabase, node["user-database"]);
  }
};

extern Settings settings;
extern std::string session;
void initDatabase();