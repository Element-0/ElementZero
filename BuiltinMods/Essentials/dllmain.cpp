#include "pch.h"
#include "global.h"
#include <dllentry.h>

Settings settings;

DEFAULT_SETTINGS(settings);

void PreInit() {
  if (settings.force_experimental_gameplay) {
    *GetServerSymbol<bool>("?mAllowExperimental@Enchant@@1_NA") = true;
    *GetServerSymbol<bool>("?mAllowExperimental@Item@@2_NA")    = true;
  }
  if (settings.education_feature) {
    GetServerSymbolWithOffset<PatchSpan<2>>(
        "?setupStandardServer@ServerCommands@@YAXAEAVMinecraft@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@"
        "D@2@@std@@1PEAVPermissionsFile@@@Z",
        0x26E)
        ->VerifyPatchFunction({0x74, 0x6D}, {0x90, 0x90});
  }
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