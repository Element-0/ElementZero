#include <sstream>

#include <log.h>
#include <scriptapi.h>

#include "global.h"

void installConsoleAPI(JsObjectWarpper &global);

void initBasicAPI() {
  DEF_LOGGER("ScriptAPI");
  auto global             = JsObjectWarpper::FromGlobal();
  global["isElementZero"] = true;
  // installConsoleAPI(global);
  for (auto [name, fn] : RegisterQueue::GetList()) {
    LOGV("register %s") % name;
    fn(global);
  }
  RegisterQueue::GetList().clear();
}