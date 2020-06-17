#include <sstream>

#include <base/log.h>
#include <base/scheduler.h>
#include <mods/ScriptSupport/scriptapi.h>

#include "global.h"

using namespace Mod::Scripting;

void installConsoleAPI(JsObjectWrapper &global);

void PromiseContinuationCallback(JsValueRef task, void *callbackState) {
  using namespace Mod::Scheduler;
  Mod::Scheduler::SetTimeOut(0_tick, [holder{ValueHolder{task}}](auto) {
    JsValueRef ref{GetUndefined()};
    JsCallFunction(*holder, &ref, 1, nullptr);
  });
}

void initBasicAPI() {
  DEF_LOGGER("ScriptAPI");
  JsSetPromiseContinuationCallback(PromiseContinuationCallback, nullptr);
  auto global             = JsObjectWrapper::FromGlobal();
  global["isElementZero"] = true;
  for (auto [name, fn] : RegisterQueue::GetList()) {
    LOGV("register %s") % name;
    fn(global);
  }
  RegisterQueue::GetList().clear();
}