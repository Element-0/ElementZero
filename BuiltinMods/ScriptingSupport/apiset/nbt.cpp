#include <Core/NBT.h>

#include <chakra_helper.h>
#include <log.h>
#include <modutils.h>
#include <memory>
#include <scriptapi.h>

namespace Mod::Scripting {

JsValueRef ScriptNBT::InitProto() {
  static ValueHolder temp = IIFE([] {
    JsObjectWarpper proto;
    proto["type"] = JsObjectWarpper::PropertyDesc{&ScriptNBT::type};
    proto["toString"] = &ScriptNBT::toString;
    proto["equals"] = &ScriptNBT::equals;
    proto["clone"] = &ScriptNBT::clone;
    return *proto;
  });
  return *temp;
}

} // namespace Mod::Scripting