#include <playerdb.h>

#include <scriptapi.h>

static ModuleRegister reg("ez:player", [](JsObjectWarpper native) -> std::string {
  native["test"] = true;
  native["fn"]   = [](JsValueRef callee, bool isConstructCall, JsConvertible::Arguments args) {
    return "From native"_js;
  };
  return R"(
    export const test = import.meta.native.test;
    export const fn = import.meta.native.fn;
  )";
});