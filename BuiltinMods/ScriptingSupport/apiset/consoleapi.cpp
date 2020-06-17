#include <sstream>
#include <stdexcept>

#include <mods/ScriptSupport/scriptapi.h>
#include <base/log.h>

using namespace Mod::Scripting;

static RegisterQueue queue("console", [](JsObjectWrapper global) {
  DEF_LOGGER("ScriptOut");
  JsObjectWrapper console;
  console["log"] = [](JsValueRef callee, Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) { ss << JsToString(arg) << " "; }
      LOGI("%s") % ss.str();
    }
    return GetUndefined();
  };
  console["warn"] = [](JsValueRef callee, Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) { ss << JsToString(arg) << " "; }
      LOGW("%s") % ss.str();
    }
    return GetUndefined();
  };
  console["error"] = [](JsValueRef callee, Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) { ss << JsToString(arg) << " "; }
      LOGE("%s") % ss.str();
    }
    return GetUndefined();
  };
  global["console"] = console;
});