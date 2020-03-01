#pragma once

#include <string>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>
#include <yaml.h>

struct Settings {
  std::string Database = "audit.db";

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    f(self.Database, node["database"]);
    return true;
  }
};

class CommandRegistry;

extern Settings settings;
extern std::unique_ptr<SQLite::Database> database;
extern bool disable_temporary;

void InitDatabase();
void InitCommand(CommandRegistry *registry);