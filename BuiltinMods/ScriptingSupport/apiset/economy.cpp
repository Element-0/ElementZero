#include <stdexcept>

#include <mods/ScriptSupport/scriptapi.h>
#include <mods/Economy.h>

#include "../lazyapi.h"

using namespace Mod::Scripting;

static LazyModuleRegister reg("ez:economy", "Economy", [](JsObjectWrapper native) -> std::string {
  native["getBalance"]    = +[](Mod::PlayerEntry const &ent) { return (double) Mod::Economy::GetBalance(ent.player); };
  native["updateBalance"] = +[](Mod::PlayerEntry const &ent, double change, std::string const &reason) {
    auto opt = Mod::Economy::UpdateBalance(ent.player, change, reason);
    if (opt) { throw std::runtime_error{*opt}; }
  };
  return R"js(
    export const getBalance = import.meta.native.getBalance;
    export const updateBalance = import.meta.native.updateBalance;
  )js";
});