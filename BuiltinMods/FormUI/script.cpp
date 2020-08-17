#include <mods/ScriptSupport/scriptapi.h>

#include "include/mods/FormUI.h"

using namespace Mod::Scripting;
using namespace Mod::FormUI;

static ModuleRegister reg("ez:formui", [](JsObjectWrapper native) -> std::string {
  native["send"] = +[](Mod::PlayerEntry const &entry, Json::Value const &def, JsValueRef func) {
    SendModalForm(entry, def, [cb = ValueHolder{func}](Json::Value const &ret) {
      try {
        JsValueRef arr[] = {GetUndefined(), ToJs(ret)};
        JsValueRef res;
        ThrowError(JsCallFunction(*cb, arr, 2, &res));
      } catch (...) {}
    });
  };
  return "export const send = import.meta.native.send;";
});
