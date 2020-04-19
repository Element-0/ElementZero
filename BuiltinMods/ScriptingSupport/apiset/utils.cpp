#include <chakra_helper.h>

#include <scriptapi.h>

using namespace Mod::Scripting;

static ModuleRegister reg("ez:utils", [](JsObjectWarpper native) -> std::string {
  return R"js(
    export function delay(time) {
      return new Promise(resolve => setTimeout(resolve, time));
    }
  )js";
});