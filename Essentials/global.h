#pragma once

#include "pch.h"

struct Settings {
  struct Commands {
    bool transferserver = true;
    bool customname     = true;
    bool teleport       = true;

    template <typename IO> static inline bool io(IO f, Commands &commands, YAML::Node &node) {
      f(commands.transferserver, node["transferserver"]);
      f(commands.customname, node["custom-name"]);
      f(commands.teleport, node["tpa"]);
      return true;
    }
  } commands;
  bool force_experimental_gameplay = false;
  bool education_feature           = false;
  bool debug_packs                 = false;
  bool load_scripts                = true;
  std::string database             = "essentials.db";

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    f(settings.commands, node["commands"]);
    f(settings.force_experimental_gameplay, node["force-experimental-gameplay"]);
    f(settings.education_feature, node["education-feature"]);
    f(settings.debug_packs, node["debug-packs"]);
    f(settings.database, node["database"]);
    return true;
  }
};

extern Settings settings;

void registerTransferServer(CommandRegistry *registry);
void registerCustomName(CommandRegistry *registry);
void registerTeleport(CommandRegistry *registry);

void loadCustomScript();

extern std::unique_ptr<SQLite::Database> database;