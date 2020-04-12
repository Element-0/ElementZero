#include <Item/ItemStack.h>
#include <chakra_helper.h>
#include <log.h>
#include <scriptapi.h>

namespace Mod::Scripting {

struct ScriptItemStack : ScriptNBT {
  const std::string name, hoverName;
  const int stackSize, maxStackSize;
  ScriptItemStack(ItemStack const &stack)
      : ScriptNBT(stack.save()), name(stack.getName()), hoverName(stack.getHoverName()),
        stackSize(stack.getStackSize()), maxStackSize(stack.getMaxStackSize()) {}

  static JsValueRef InitProto();

  inline static JsObjectWarpper Create(ItemStack const &stack) {
    return JsObjectWarpper::FromExternalObject(new ScriptItemStack{stack});
  }
};
JsValueRef ToJs(ItemStack const &stack) {
  if (stack.isNull()) return GetNullValue();
  return *ScriptItemStack::Create(stack);
}
template <> struct HasToJs<ItemStack> : std::true_type {};

JsValueRef ScriptItemStack::InitProto() {
  static ValueHolder temp = IIFE([] {
    JsObjectWarpper proto;
    proto.SetPrototype(ScriptNBT::InitProto());
    proto["name"]       = JsObjectWarpper::PropertyDesc{&ScriptItemStack::name};
    proto["hover_name"] = JsObjectWarpper::PropertyDesc{&ScriptItemStack::hoverName};
    proto["stack_size"] = JsObjectWarpper::PropertyDesc{&ScriptItemStack::stackSize};
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