#include <Script/ScriptObjectHandle.h>
#include <Script/ScriptEngine.h>
#include <Core/json.h>

#include <chakra_helper.h>
#include <scriptapi.h>
#include <vector>

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