#include <memory>

#include <base.h>

#include <modutils.h>
#include <scriptapi.h>
#include <command.h>
#include <stdexcept>

using namespace Mod::Scripting;

static ModuleRegister reg("ez:command", [](JsObjectWrapper native) -> std::string {
  native["executeCommand"] = +[](std::string const &command) {
    return ToJs(
        Mod::CommandSupport::GetInstance().ExecuteCommand(std::make_unique<Mod::CustomCommandOrigin>(), command));
    // auto origin      = std::make_unique<Mod::CustomCommandOrigin>();
    // origin->callback = [holder = ValueHolder{cb}](Json::Value &&value) mutable {
    //   try {
    //     auto json         = ToJs(value);
    //     JsValueRef args[] = {GetUndefined(), json};
    //     JsCallFunction(*holder, args, 2, nullptr);
    //   } catch (...) {}
    // };
    // auto ctx    = CommandContext::create(data, std::move(origin));
    // auto result = LocateService<MinecraftCommands>()->executeCommand(std::move(ctx), false);
    // return result == MCRESULT_Success;
  };
  return R"js(
    export const executeCommand = import.meta.native.executeCommand;
  )js";
});
