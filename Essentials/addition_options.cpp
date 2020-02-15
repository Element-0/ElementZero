#include "pch.h"

#include "global.h"

#include <Core/core.h>

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

TClasslessInstanceHook(void, "?initialize@ScriptEngine@@UEAA_NXZ") {
  DEF_LOGGER("ScriptEngine");
  LOGV("initialize");
  original(this);
}

TClasslessInstanceHook(bool, "?_processSystemInitialize@ScriptEngine@@AEAA_NXZ") {
  DEF_LOGGER("ScriptEngine");
  auto ret = original(this);
  LOGV("systemInitialize: %d") % ret;
  return ret;
}

struct ScriptQueueData {
  std::string relative_path, virtual_path, content, uuid, version_code;
};

TClasslessInstanceHook(bool, "?_runScript@ScriptEngine@@AEAA_NAEBUScriptQueueData@1@@Z", ScriptQueueData &data) {
  DEF_LOGGER("ScriptEngine");
  auto ret = original(this, data);
  LOGV("runScript: %s -> %d") % data.virtual_path % ret;
  return ret;
}

TClasslessInstanceHook(
    bool, "?onErrorReceived@ScriptEngine@@UEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
    std::string const &err) {
  DEF_LOGGER("ScriptEngine");
  auto ret = original(this, err);
  LOGE("%s") % err;
  return ret;
}
TClasslessInstanceHook(
    bool, "?onWarnReceived@ScriptEngine@@UEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
    std::string const &err) {
  DEF_LOGGER("ScriptEngine");
  auto ret = original(this, err);
  LOGW("%s") % err;
  return ret;
}
TClasslessInstanceHook(
    bool, "?onInfoReceived@ScriptEngine@@UEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
    std::string const &err) {
  DEF_LOGGER("ScriptEngine");
  auto ret = original(this, err);
  LOGI("%s") % err;
  return ret;
}
TClasslessInstanceHook(
    bool, "?onLogReceived@ScriptEngine@@UEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
    std::string const &err) {
  DEF_LOGGER("ScriptEngine");
  auto ret = original(this, err);
  LOGV("%s") % err;
  return ret;
}

TClasslessInstanceHook(
    void,
    "?setStack@ResourcePackManager@@QEAAXV?$unique_ptr@VResourcePackStack@@U?$default_delete@VResourcePackStack@@@std@@"
    "@std@@W4ResourcePackStackType@@_N@Z",
    void *ptr, int type, bool flag) {
  ((char *) this)[227] = settings.force_experimental_gameplay;
  original(this, ptr, type, flag);
}

TClasslessInstanceHook(
    void, "?forEachIn@ResourcePack@@QEBAXAEBVPath@Core@@V?$function@$$A6AXAEBVPath@Core@@@Z@std@@H_N@Z",
    Core::Path const &path, std::function<void(Core::Path const &)> fn, int a, bool flag) {
  if (settings.debug_packs) {
    DEF_LOGGER("PACKDEBUG");
    LOGV("load packs: %p %p %d %d") % (void *) this % path.data % a % flag;
    original(
        this, path,
        [=](auto const &path) {
          LOGV("\t%s") % path.data;
          fn(path);
        },
        a, flag);
  } else {
    original(this, path, fn, a, flag);
  }
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