#include <Actor/ActorType.h>
#include <Actor/Player.h>

#include <dllentry.h>

#include <base/log.h>
#include <base/playerdb.h>
#include <mods/CommandSupport.h>
#include <mods/BossBarSupport.h>

#include "global.h"

void dllenter() {}
void dllexit() {}

DEF_LOGGER("BossBar");

static void ProcessDimension(Mod::PlayerEntry const &entry, ChangeDimensionRequest const &request, bool stage) {
  OnChangeDimension(entry.player, stage);
}

void PreInit() {
  auto &pdb = Mod::PlayerDatabase::GetInstance();
  pdb.AddListener(SIG("change_dimension"), &ProcessDimension);
}