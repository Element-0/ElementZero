#pragma once

#include <string>

#include <SQLiteCpp/SQLiteCpp.h>

#include <yaml.h>

struct Settings {
  std::string Database = "packet.db";

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.Database, node["database"]);
  }
};

inline Settings settings;
inline std::unique_ptr<SQLite::Database> database;

extern void InitDatabase();