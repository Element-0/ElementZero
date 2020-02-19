#pragma once

#include "pch.h"

struct Settings {
  struct Commands {
    bool transferserver = true;
    bool customname     = true;
    bool teleport       = true;
  } commands;
  bool force_experimental_gameplay = false;
  bool education_feature           = false;
  bool debug_packs                 = false;
  bool load_scripts                = true;
  std::string database             = "essentials.db";
};

extern Settings settings;

void registerTransferServer(CommandRegistry *registry);
void registerCustomName(CommandRegistry *registry);
void registerTeleport(CommandRegistry *registry);

void loadCustomScript();

extern std::unique_ptr<SQLite::Database> database;