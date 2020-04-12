#include "boost/format/format_fwd.hpp"
#include <Item/Enchant.h>
#include <Item/ItemStack.h>
#include <chakra_helper.h>
#include <log.h>
#include <scriptapi.h>

namespace Mod::Scripting {

JsValueRef ScriptEnchantmentInstance::InitProto() {
  static ValueHolder temp = IIFE([] {
    JsObjectWarpper proto;
    proto["type"]     = JsObjectWarpper::PropertyDesc{&ScriptEnchantmentInstance::GetType};
    proto["name"]     = JsObjectWarpper::PropertyDesc{&EnchantmentInstance::getName};
    proto["level"]    = JsObjectWarpper::PropertyDesc{&EnchantmentInstance::level};
    proto["toString"] = &EnchantmentInstance::toString;
    return *proto;
  });
  return *temp;
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
    proto["enchanted"]         = JsObjectWarpper::PropertyDesc{&ItemStack::isEnchanted};
    proto["enchants"]          = JsObjectWarpper::PropertyDesc{&ScriptItemStack::GetEnchants};
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