#include "base/base.h"
#include <stdexcept>

#include <Core/mce.h>
#include <boost/lexical_cast.hpp>

#include <Script/ScriptObjectHandle.h>
#include <Script/ScriptObjectBinder.h>
#include <Script/MinecraftServerScriptEngine.h>

#include <modutils.h>

#include <base/log.h>
#include <mods/ScriptSupport/scriptapi.h>

using namespace Mod::Scripting;

Actor *EntityBinding::TryFetch() const { return LocateService<Level>()->fetchEntity(id, true); }

Actor &EntityBinding::Fetch() const {
  if (auto ret = TryFetch(); ret) return *ret;
  throw std::runtime_error{"invalid entity"};
}

JsValueRef EntityBinding::GetVanillaObject() const {
  ScriptApi::ScriptObjectHandle handle;
  LocateService<MinecraftServerScriptEngine>()->helpDefineActor(Fetch(), handle);
  return handle.ref;
}

JsValueRef EntityBinding::InitProto() {
  static LeakedHolder proto = IIFE([] {
    JsObjectWrapper Temp;
    Temp["entityName"] = JsObjectWrapper::PropertyDesc{&EntityBinding::GetEntityName};
    Temp["nameTag"]    = JsObjectWrapper::PropertyDesc{&EntityBinding::GetNameTag, &EntityBinding::SetNameTag};
    Temp["pos"]        = JsObjectWrapper::PropertyDesc{&EntityBinding::GetPos, &EntityBinding::SetPos};
    Temp["kill"]       = &EntityBinding::Kill;
    Temp["valid"]      = JsObjectWrapper::PropertyDesc{&EntityBinding::IsValid};
    Temp["vanilla"]    = JsObjectWrapper::PropertyDesc{&EntityBinding::GetVanillaObject};
    return Temp.ref;
  });
  return *proto;
}

static ModuleRegister reg("ez:entity", [](JsObjectWrapper native) -> std::string {
  native["fromVanilla"] = +[](std::string const &name) {
    auto &engine = *LocateService<MinecraftServerScriptEngine>();
    ScriptApi::ScriptObjectHandle obj;
    auto binder = ScriptObjectBinder::extract(engine, obj);
    if (!binder) return GetUndefined();
    Actor *ret{};
    engine.helpGetActor(*binder, &ret);
    if (!ret) return GetUndefined();
    return ToJs(ret->getUniqueID());
  };

  return R"(
    export const fromVanilla = import.meta.native.fromVanilla;
  )";
});