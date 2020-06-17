#include <Script/ScriptObjectHandle.h>
#include <Script/ScriptEngine.h>
#include <Core/json.h>

#include <mods/ScriptSupport/scriptapi.h>

namespace Mod::Scripting {

JsValueRef ToJs(Json::Value entry) {
  ScriptApi::ScriptObjectHandle handle;
  LocateService<ScriptEngine>()->serializeJsonToScriptObjectHandle(handle, entry);
  return handle.ref;
}
template <> Json::Value FromJs(JsValueRef ref) {
  Json::Value ret;
  LocateService<ScriptEngine>()->deserializeScriptObjectHandleToJson({ref}, ret);
  return ret;
}

} // namespace Mod::Scripting