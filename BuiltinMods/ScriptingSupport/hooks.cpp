#include <Script/ScriptEngine.h>

#include <hook.h>
#include <log.h>
#include <scriptapi.h>

#include "global.h"

DEF_LOGGER("Scripting");

THook(bool, "?isScriptingEnabled@ScriptEngine@@SA_NXZ") { return true; }

static ScriptEngine *engine;

template <> ScriptEngine *LocateService<ScriptEngine>() { return engine; }

TInstanceHook(void, "?initialize@ScriptEngine@@UEAA_NXZ", ScriptEngine) {
  DEF_LOGGER("ScriptEngine");
  LOGV("initialize");
  original(this);
  engine = this;
  initBasicAPI();
}

TInstanceHook(int, "?startScriptLoading@ScriptEngine@@QEAAXXZ", ScriptEngine) {
  DEF_LOGGER("ScriptEngine");
  LOGV("loading");
  loadCustomScript();
  return original(this);
}

TClasslessInstanceHook(bool, "?_processSystemInitialize@ScriptEngine@@AEAA_NXZ") {
  DEF_LOGGER("ScriptEngine");
  auto ret = original(this);
  LOGV("systemInitialize: %d") % ret;
  return ret;
}

TClasslessInstanceHook(
    bool, "?_runScript@ScriptEngine@@AEAA_NAEBUScriptQueueData@1@@Z", ScriptEngine::ScriptQueueData &data) {
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