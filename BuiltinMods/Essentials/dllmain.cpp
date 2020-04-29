#include "pch.h"
#include "global.h"
#include <dllentry.h>

Settings settings;

DEFAULT_SETTINGS(settings);

void PreInit() {
  database = std::make_unique<SQLite::Database>(settings.database, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
  initDatabase();
}

void WorldInit(std::filesystem::path const &path) { initWorldDatabase(path); }

void dllenter() {
  Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), [](auto x) {
    if (settings.commands.transferserver) registerTransferServer(x);
    if (settings.commands.customname) registerCustomName(x);
    if (settings.commands.teleport.enabled) registerTeleport(x);
    if (settings.commands.home) registerHome(x);
    if (settings.commands.warp) registerWarp(x);
  });
}
void dllexit() {}