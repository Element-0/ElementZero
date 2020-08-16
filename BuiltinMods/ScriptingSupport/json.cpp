#include <Script/ScriptObjectHandle.h>
#include <Script/MinecraftServerScriptEngine.h>
#include <Core/json.h>

#include <mods/ScriptSupport/scriptapi.h>

namespace Mod::Scripting {

JsValueRef ToJs(Json::Value entry) {
  ScriptApi::ScriptObjectHandle handle;
  LocateService<MinecraftServerScriptEngine>()->serializeJsonToScriptObjectHandle(handle, entry);
  return handle.ref;
}
template <> Json::Value FromJs(JsValueRef ref) {
  Json::Value ret;
  LocateService<MinecraftServerScriptEngine>()->deserializeScriptObjectHandleToJson({ref}, ret);
  return ret;
}

} // namespace Mod::Scripting