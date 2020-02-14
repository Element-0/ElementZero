#include "pch.h"

#include "global.h"

TClasslessInstanceHook(bool, "?_isFeatureEnabled@EducationOptions@@AEBA_NW4EducationFeature@@@Z", int v) {
  if (settings.education_feature) return true;
  return original(this, v);
}
TClasslessInstanceHook(bool, "?isChemistryEnabled@EducationOptions@@SA_NXZ") {
  if (settings.education_feature) return true;
  return original(this);
}
TClasslessInstanceHook(bool, "?isEnabled@FeatureToggles@@QEBA_NW4FeatureOptionID@@@Z", int id) {
  if (settings.force_experimental_gameplay) return true;
  return original(this, id);
}
THook(void, "??0LevelSettings@@QEAA@AEBV0@@Z", char *lhs, char *rhs) {
  rhs[76] = settings.force_experimental_gameplay;
  rhs[36] = settings.education_feature;
  original(lhs, rhs);
}

TClasslessInstanceHook(
    void,
    "??0LevelSettings@@QEAA@IW4GameType@@W4Difficulty@@V?$AutomaticID@VDimension@@H@@W4GeneratorType@@AEBVBlockPos@@_"
    "NHW4EducationEditionOffer@@MM555W4GamePublishSetting@Social@@7555555VGameRules@@55VAbilities@@"
    "I555AEBVBaseGameVersion@@@Z",
    unsigned int seed, int gametype, int difficulty, int def_dim, int generator, void *blockpos,
    bool achievementsDisabled, int time, int eduoffer, float rainLevel, float lightningLevel, bool flag0, bool flag1,
    bool flag2, int xblpublishsetting, int platformpublishsetting, bool flag3, bool flag4, bool flag5, bool flag6,
    bool flag7, bool flag8, void *rules, bool flag9, bool flag10, void *abilities, unsigned unk1, bool flag11,
    bool flag12, bool flag13, void *version) {
  original(
      this, seed, gametype, difficulty, def_dim, generator, blockpos, achievementsDisabled, time,
      settings.education_feature ? 1 : 0, rainLevel, lightningLevel, flag0, flag0, flag2, xblpublishsetting,
      platformpublishsetting, flag3, flag4, flag5, flag6, flag7, flag8, rules, flag9, flag10, abilities, unk1, flag11,
      flag12, flag13, version);
  char *access = (char *) this;
  access[76]   = settings.force_experimental_gameplay;
  access[36]   = settings.education_feature;
}