#include <yaml.h>
#include <dllentry.h>

#include <mods/CommandSupport.h>

#include "global.h"

Settings settings;
std::unique_ptr<SQLite::Database> db;

DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

void PreInit() {
  InitDatabase();
  if (settings.command) Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), InitCommand);
}