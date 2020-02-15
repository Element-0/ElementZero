#pragma once

#include "pch.h"

extern struct Settings {
  struct Commands {
    bool transferserver = true;
  } commands;
  bool force_experimental_gameplay = false;
  bool education_feature           = false;
  bool debug_packs                 = false;
} settings;

void startRegister(CommandRegistry *registry);