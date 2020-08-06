#include <memory>

#include <Core/NBT.h>

#include <modutils.h>

#include <base/log.h>
#include <mods/ScriptSupport/scriptapi.h>

namespace Mod::Scripting {

JsValueRef ScriptNBT::InitProto() {
  static LeakedHolder temp = IIFE([] {
    JsObjectWrapper proto;
    proto["type"] = JsObjectWrapper::PropertyDesc{&ScriptNBT::type};
    proto["toString"] = &ScriptNBT::toString;
    proto["equals"] = &ScriptNBT::equals;
    proto["clone"] = &ScriptNBT::clone;
    return *proto;
  });
  return *temp;
}

} // namespace Mod::Scripting