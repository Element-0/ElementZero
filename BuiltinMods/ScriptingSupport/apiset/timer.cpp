#include <sstream>

#include <ChakraCommon.h>

#include <base/log.h>
#include <base/scheduler.h>
#include <mods/ScriptSupport/scriptapi.h>

using namespace Mod::Scripting;

static RegisterQueue queue("timer", [](JsObjectWrapper global) {
  global["setTimeout"] = [](JsValueRef callee, Arguments args) -> JsValueRef {
    if (args.size() != 1 && args.size() != 2) throw std::runtime_error{"Require 1 or 2 arguments"};
    auto fn = args[0];
    if (GetJsType(fn) != JsFunction) throw std::runtime_error{"The first argument require to be function"};
    auto delay = args.size() == 2 ? FromJs<int>(args[1]) : 0;
    auto token =
        Mod::Scheduler::SetTimeOut(Mod::Scheduler::GameTick(delay), [holder{ValueHolder{fn}}](unsigned long long) {
          JsValueRef ref{GetUndefined()};
          JsCallFunction(*holder, &ref, 1, nullptr);
        });
    return ToJs((double) token);
  };
  global["clearTimeout"] = +[](double tok) {
    Mod::Scheduler::Token token = (size_t) tok;
    Mod::Scheduler::ClearTimeOut(token);
  };

  global["setInterval"] = [](JsValueRef callee, Arguments args) -> JsValueRef {
    if (args.size() != 1 && args.size() != 2) throw std::runtime_error{"Require 1 or 2 arguments"};
    auto fn = args[0];
    if (GetJsType(fn) != JsFunction) throw std::runtime_error{"The first argument require to be function"};
    auto delay = args.size() == 2 ? FromJs<int>(args[1]) : 1;
    auto token = Mod::Scheduler::SetInterval(
        Mod::Scheduler::GameTick(delay <= 0 ? 1 : delay), [holder{ValueHolder{fn}}](unsigned long long) {
          JsValueRef ref{GetUndefined()};
          JsCallFunction(*holder, &ref, 1, nullptr);
        });
    return ToJs((double) token);
  };
  global["clearInterval"] = +[](double tok) {
    Mod::Scheduler::Token token = (size_t) tok;
    Mod::Scheduler::ClearInterval(token);
  };
});