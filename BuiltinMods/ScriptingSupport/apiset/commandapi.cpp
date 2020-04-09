#include <memory>

#include <base.h>
#include <Core/json.h>
#include <Core/MCRESULT.h>
#include <Command/CommandContext.h>
#include <Command/MinecraftCommands.h>

#include <modutils.h>
#include <scriptapi.h>
#include <command.h>
#include <stdexcept>

using namespace Mod::Scripting;

static ModuleRegister reg("ez:command", [](JsObjectWarpper native) -> std::string {
  native["executeCommand"] = +[](std::string const &data, JsValueRef cb) {
    if (GetJsType(cb) != JsFunction) throw std::runtime_error{"require function"};
    auto origin      = std::make_unique<Mod::CustomCommandOrigin>();
    origin->callback = [holder = ValueHolder{cb}](Json::Value &&value) mutable {
      printf("here\n");
      try {
        auto json         = ToJs(value);
        JsValueRef args[] = {GetUndefined(), json};
        JsCallFunction(*holder, args, 2, nullptr);
      } catch (...) {}
    };
    auto ctx    = CommandContext::create(data, std::move(origin));
    auto result = LocateService<MinecraftCommands>()->executeCommand(std::move(ctx), false);
    return result == MCRESULT_Success;
  };
  return R"js(
    export const executeCommand = import.meta.native.executeCommand;
  )js";
});
