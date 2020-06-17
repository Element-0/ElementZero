#include <mods/ScriptSupport/scriptapi.h>

using namespace Mod::Scripting;

static ModuleRegister reg("ez:utils", [](JsObjectWrapper native) -> std::string {
  return R"js(
    export function delay(time) {
      return new Promise(resolve => setTimeout(resolve, time));
    }
  )js";
});