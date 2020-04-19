#include <dllentry.h>
#include <command.h>
#include <log.h>

#include <Actor/ActorType.h>
#include <Actor/Player.h>
#include <bossbar.h>
#include <playerdb.h>

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