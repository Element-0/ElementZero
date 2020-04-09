#include <sstream>

#include "chakra_helper.h"
#include "log.h"

#include <scriptapi.h>

using namespace Mod::Scripting;

static RegisterQueue queue("console", [](JsObjectWarpper global) {
  DEF_LOGGER("ScriptOut");
  JsObjectWarpper console;
  console["log"] = [](JsValueRef callee, Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) {
        ss << JsToString(arg) << " ";
      }
      LOGI("%s") % ss.str();
    }
    return GetUndefined();
  };
  console["warn"] = [](JsValueRef callee, Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) {
        ss << JsToString(arg) << " ";
      }
      LOGW("%s") % ss.str();
    }
    return GetUndefined();
  };
  console["error"] = [](JsValueRef callee, Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) {
        ss << JsToString(arg) << " ";
      }
      LOGE("%s") % ss.str();
    }
    return GetUndefined();
  };
  global["console"] = console;
});