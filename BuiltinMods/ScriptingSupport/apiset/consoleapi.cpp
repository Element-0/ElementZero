#include <sstream>

#include "log.h"

#include <scriptapi.h>

static RegisterQueue queue("console", [](JsObjectWarpper global) {
  DEF_LOGGER("ScriptOut");
  JsObjectWarpper console;
  console["log"] = [](JsValueRef callee, bool isConstructCall, JsConvertible::Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) {
        JsValueRef str;
        JsConvertValueToString(arg, &str);
        ss << FromJs<std::string>(str);
      }
      LOGI("%s") % ss.str();
    }
    return GetUndefined();
  };
  console["warn"] = [](JsValueRef callee, bool isConstructCall, JsConvertible::Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) {
        JsValueRef str;
        JsConvertValueToString(arg, &str);
        ss << FromJs<std::string>(str);
      }
      LOGW("%s") % ss.str();
    }
    return GetUndefined();
  };
  console["error"] = [](JsValueRef callee, bool isConstructCall, JsConvertible::Arguments args) {
    if (!args.empty()) {
      std::stringstream ss;
      for (auto arg : args) {
        JsValueRef str;
        JsConvertValueToString(arg, &str);
        ss << FromJs<std::string>(str);
      }
      LOGE("%s") % ss.str();
    }
    return GetUndefined();
  };
  global["console"] = console;
});