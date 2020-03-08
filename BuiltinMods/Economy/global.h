#pragma once
#include <string>

#include <Command/Command.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include <yaml.h>

struct Settings {
  std::string database = "economy.db";
  bool command         = true;

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.database, node["database"]) && f(self.command, node["command"]);
  }
};

extern Settings settings;
extern std::unique_ptr<SQLite::Database> db;

extern void InitDatabase();
extern void InitCommand(CommandRegistry *registry);