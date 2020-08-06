#include <Core/DataIO.h>
#include <Item/Enchant.h>
#include <Item/ItemStack.h>

#include <base/log.h>

#include <mods/ScriptSupport/scriptapi.h>

namespace Mod::Scripting {

JsValueRef ScriptEnchantmentInstance::InitProto() {
  static LeakedHolder temp = IIFE([] {
    JsObjectWrapper proto;
    proto["type"]     = JsObjectWrapper::PropertyDesc{&ScriptEnchantmentInstance::GetType};
    proto["name"]     = JsObjectWrapper::PropertyDesc{&EnchantmentInstance::getName};
    proto["level"]    = JsObjectWrapper::PropertyDesc{&EnchantmentInstance::level};
    proto["toString"] = &EnchantmentInstance::toString;
    return *proto;
  });
  return *temp;
}

JsValueRef ScriptItemStack::Dump() const {
  std::string buffer;
  StringByteOutput output{buffer};
  stack.save()->write(output);
  JsValueRef ret;
  JsCreateArrayBuffer(buffer.size(), &ret);
  ChakraBytePtr ptr;
  unsigned int _len;
  JsGetArrayBufferStorage(ret, &ptr, &_len);
  memcpy(ptr, buffer.data(), buffer.size());
  return ret;
}

JsValueRef ScriptItemStack::InitProto() {
  static LeakedHolder temp = IIFE([] {
    JsObjectWrapper proto;
    proto["raw_name"]          = JsObjectWrapper::PropertyDesc{&ItemStack::getRawNameId};
    proto["name"]              = JsObjectWrapper::PropertyDesc{&ItemStack::getName};
    proto["hover_name"]        = JsObjectWrapper::PropertyDesc{&ItemStack::getHoverName};
    proto["custom_hover_name"] = JsObjectWrapper::PropertyDesc{&ItemStack::hasCustomHoverName};
    proto["id"]                = JsObjectWrapper::PropertyDesc{&ItemStack::getId};
    proto["aux"]               = JsObjectWrapper::PropertyDesc{&ItemStack::getAuxValue};
    proto["count"]             = JsObjectWrapper::PropertyDesc{&ItemStack::getStackSize};
    proto["max_count"]         = JsObjectWrapper::PropertyDesc{&ItemStack::getMaxStackSize};
    proto["enchanted"]         = JsObjectWrapper::PropertyDesc{&ItemStack::isEnchanted};
    proto["enchants"]          = JsObjectWrapper::PropertyDesc{&ScriptItemStack::GetEnchants};
    proto["dump"]              = &ScriptItemStack::Dump;
    proto["equals"]            = &ScriptItemStack::Equals;
    proto["toString"]          = &ItemStackBase::toString;
    return *proto;
  });
  return *temp;
}

} // namespace Mod::Scripting

using namespace Mod::Scripting;

static ModuleRegister reg("ez:inventory", [](JsObjectWrapper native) -> std::string {
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
  native["getEnderChestItems"] = +[](Mod::PlayerEntry entry) {
    auto &container = *entry.player->getEnderChestContainer();
    return ToJsArray(container.data);
  };
  return R"js(
    export const getOffHandItem = import.meta.native.getOffHandItem;
    export const getEquipmentItems = import.meta.native.getEquipmentItems;
    export const getInventoryItems = import.meta.native.getInventoryItems;
    export const getEnderChestItems = import.meta.native.getEnderChestItems;
  )js";
});