#pragma once

#include "pch.h"

extern struct Settings {
  struct Commands {
    bool transferserver = true;
    bool customname     = true;
  } commands;
  bool force_experimental_gameplay = false;
  bool education_feature           = false;
  bool debug_packs                 = false;
  std::string database             = "essentials.db";
} settings;

void registerTransferServer(CommandRegistry *registry);
void registerCustomName(CommandRegistry *registry);

extern std::unique_ptr<SQLite::Database> database;