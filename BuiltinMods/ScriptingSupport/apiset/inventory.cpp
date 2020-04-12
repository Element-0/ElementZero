#include "boost/format/format_fwd.hpp"
#include <Item/ItemStack.h>
#include <chakra_helper.h>
#include <log.h>
#include <scriptapi.h>

namespace Mod::Scripting {

struct ScriptItemStack {
  ItemStack stack;
  ScriptItemStack(ItemStack const &stack) : stack(stack) {}

  bool Equals(ItemStack const &rhs) { return !(stack != rhs); }

  static JsValueRef InitProto();

  inline static JsObjectWarpper Create(ItemStack const &stack) {
    return JsObjectWarpper::FromExternalObject(new ScriptItemStack{stack}, InitProto());
  }
};
JsValueRef ToJs(ItemStack const &stack) {
  if (stack.isNull()) return GetNullValue();
  return *ScriptItemStack::Create(stack);
}
template <> struct HasToJs<ItemStack> : std::true_type {};
template <> ItemStack FromJs(JsValueRef ref) {
  if (GetJsType(ref) == JsNull) return ItemStack{};
  JsValueRef tmp;
  ThrowError(JsGetPrototype(ref, &tmp));
  if (tmp == ScriptItemStack::InitProto()) {
    ScriptItemStack *bd;
    ThrowError(JsGetExternalData(ref, (void **) &bd));
    if (bd) return bd->stack;
  }
  throw std::runtime_error{"null data"};
}

JsValueRef ScriptItemStack::InitProto() {
  static ValueHolder temp = IIFE([] {
    JsObjectWarpper proto;
    proto["raw_name"]          = JsObjectWarpper::PropertyDesc{&ItemStack::getRawNameId};
    proto["name"]              = JsObjectWarpper::PropertyDesc{&ItemStack::getName};
    proto["hover_name"]        = JsObjectWarpper::PropertyDesc{&ItemStack::getHoverName};
    proto["custom_hover_name"] = JsObjectWarpper::PropertyDesc{&ItemStack::hasCustomHoverName};
    proto["id"]                = JsObjectWarpper::PropertyDesc{&ItemStack::getId};
    proto["aux"]               = JsObjectWarpper::PropertyDesc{&ItemStack::getAuxValue};
    proto["count"]             = JsObjectWarpper::PropertyDesc{&ItemStack::getStackSize};
    proto["max_count"]         = JsObjectWarpper::PropertyDesc{&ItemStack::getMaxStackSize};
    proto["equals"]            = &ScriptItemStack::Equals;
    proto["toString"]          = &ItemStackBase::toString;
    return *proto;
  });
  return *temp;
}

} // namespace Mod::Scripting

using namespace Mod::Scripting;

static ModuleRegister reg("ez:inventory", [](JsObjectWarpper native) -> std::string {
  native["getOffHandItem"] = +[](Mod::PlayerEntry entry) {
    auto &container = entry.player->getHandContainer();
    return ToJs(container.data[1]);
  };
  native["getEquipmentItems"] = +[](Mod::PlayerEntry entry) {
    auto &container = entry.player->getEquipmentContainer();
    return ToJsArray(container.data);
  };
  native["getInventoryItems"] = +[](Mod::PlayerEntry entry) {
    auto &container = *entry.player->getInventory().invectory;
    return ToJsArray(container.data);
  };
  return R"js(
    export const getOffHandItem = import.meta.native.getOffHandItem;
    export const getEquipmentItems = import.meta.native.getEquipmentItems;
    export const getInventoryItems = import.meta.native.getInventoryItems;
  )js";
});