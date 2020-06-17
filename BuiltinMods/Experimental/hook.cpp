#include <functional>

#include <Core/core.h>

#include <hook.h>

#include <base/log.h>

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