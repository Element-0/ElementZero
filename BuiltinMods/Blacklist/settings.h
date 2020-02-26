#pragma once

#include <yaml.h>
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>

struct Settings {
  std::string Database = "blacklist.db";

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.Database, node["database"]);
  }
};

extern Settings settings;
extern std::unique_ptr<SQLite::Database> database;