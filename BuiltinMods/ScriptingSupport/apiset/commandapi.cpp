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
  };
  return R"js(
    export const executeCommand = import.meta.native.executeCommand;
  )js";
});
