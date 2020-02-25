#include "pch.h"
#include "global.h"
#include <dllentry.h>

Settings settings;
std::unique_ptr<SQLite::Database> database;

DEFAULT_SETTINGS(settings);

void PreInit() {
  if (settings.force_experimental_gameplay) {
    *GetServerSymbol<bool>("?mAllowExperimental@Enchant@@1_NA") = true;
    *GetServerSymbol<bool>("?mAllowExperimental@Item@@2_NA")    = true;
  }
  database = std::make_unique<SQLite::Database>(settings.database, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
}

void dllenter() {
  Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), [](auto x) {
    if (settings.commands.transferserver) registerTransferServer(x);
    if (settings.commands.customname) registerCustomName(x);
    if (settings.commands.teleport) registerTeleport(x);
  });
}
void dllexit() {}