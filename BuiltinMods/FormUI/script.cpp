#include <mods/ScriptSupport/scriptapi.h>

#include <base/log.h>

#include "include/mods/FormUI.h"

using namespace Mod::Scripting;

DEF_LOGGER("FormUI");

static std::function<void(Json::Value const &)> GetCallback(JsValueRef func) {
  return [cb = ValueHolder{func}](Json::Value const &ret) {
    try {
      JsValueRef arr[] = {GetUndefined(), ToJs(ret)};
      JsValueRef res;
      ThrowError(JsCallFunction(*cb, arr, 2, &res));
    } catch (std::exception const &ex) { LOGE("error in script: %s") % ex.what(); } catch (Exception ex) {
      LOGE("script exception: %s") % JsToString(ex.raw);
    }
  };
}

static ModuleRegister reg("ez:formui", [](JsObjectWrapper native) -> std::string {
  native["send"] = +[](Mod::PlayerEntry const &entry, Json::Value const &def, JsValueRef func) {
    Mod::FormUI::GetInstance().SendModalForm(entry, def, GetCallback(func));
  };
  native["onServerSettings"] = +[](JsValueRef func) {
    Mod::FormUI::GetInstance().AddListener(
        SIG("server_settings"),
        [cb = LeakedHolder{func}](
            Mod::PlayerEntry const &entry,
            Mod::CallbackToken<std::pair<Json::Value, std::function<void(Json::Value const &)>>> &token) {
          try {
            JsValueRef arr[] = {GetUndefined(), ToJs(entry)};
            JsValueRef res;
            ThrowError(JsCallFunction(*cb, arr, 2, &res));
            if (GetJsType(res) == JsArray) {
              JsObjectWrapper wrap{res};
              auto json = wrap[0].get<Json::Value>();
              token({json, GetCallback(*wrap[1])});
            }
          } catch (...) {
            LOGE("unknown error");
          }
        });
  };
  return R"(
    export const send = import.meta.native.send;
    export const onServerSettings = import.meta.native.onServerSettings;
  )";
});
