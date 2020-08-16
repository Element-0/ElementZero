#include <memory>
#include <stdexcept>

#include <modutils.h>

#include <Math/Vec3.h>
#include <Math/BlockPos.h>
#include <Level/Dimension.h>

#include <hook.h>
#include <base/base.h>
#include <base/log.h>
#include <mods/ScriptSupport/scriptapi.h>
#include <mods/CommandSupport.h>

using namespace Mod::Scripting;

DEF_LOGGER("slash_command");
static LeakedHolder holder;

static CommandOutputParameter ToCommandOutputParameter(JsValueRef val) {
  switch (GetJsType(val)) {
  case JsString: return {FromJs<std::string>(val)};
  case JsNumber: return {FromJs<double>(val)};
  default: return {JsToString(val)};
  }
}

template <typename T> static JsValueRef VecToJs(T const &vec) {
  JsObjectWrapper obj;
  obj["x"] = vec.x;
  obj["y"] = vec.y;
  obj["z"] = vec.z;
  return obj.ref;
}

static JsValueRef DumpCommandOrigin(CommandOrigin const &orig) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  JsObjectWrapper obj;
  obj["type"] = (int) orig.getOriginType();
  if (auto entity = orig.getEntity(); entity) {
    if (auto player = db.Find((Player *) entity); player) { obj["player"] = *player; }
  }
  obj["name"] = orig.getName();
  if (auto dim = orig.getDimension(); dim) obj["dimension"] = dim->DimensionId.value;
  obj["permission"]   = (int) orig.getPermissionsLevel();
  obj["worldBuilder"] = orig.isWorldBuilder();
  obj["blockpos"]     = VecToJs(orig.getBlockPosition());
  obj["worldpos"]     = VecToJs(orig.getWorldPosition());
  return *obj;
}

struct SlashCommand : Command {
  CommandRawText content;

  void execute(const CommandOrigin &orig, CommandOutput &outp) override {
    if (!holder) {
      outp.error("commands.error.not.implemented");
      return;
    }
    try {
      JsValueRef arr[] = {DumpCommandOrigin(orig), ToJs(content.getText())};
      JsValueRef ret;
      ThrowError(JsCallFunction(*holder, arr, 2, &ret));
      switch (GetJsType(ret)) {
      case JsUndefined:
      case JsNull: outp.success(); return;
      case JsArray: {
        JsObjectWrapper arr{ret};
        auto len = arr["length"].get<int>();
        switch (len) {
        case 0: outp.success(); return;
        case 1: outp.success(JsToString(*arr[0])); return;
        default: {
          std::vector<CommandOutputParameter> params;
          for (int i = 1; i < len; i++) { params.emplace_back(ToCommandOutputParameter(*arr[i])); }
          outp.success(JsToString(*arr[0]), std::move(params));
          return;
        }
        }
        return;
      }
      case JsString: outp.success(FromJs<std::string>(ret)); return;
      default: outp.success(JsToString(ret)); return;
      }
    } catch (std::exception const &ex) { outp.error(ex.what()); }
  }

  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "!", "commands.slash.description", CommandPermissionLevel::Any, CommandFlagCheat, CommandFlagNone);
    registry->registerAlias("!", "slash");
    registry->registerOverload<SlashCommand>("!", optional(&SlashCommand::content, "content"));
  }
};

TClasslessInstanceHook(
    void *, "?parse@Parser@CommandRegistry@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
    std::string &input) {
  if (input._Starts_with("/!")) { input.replace(0, 2, "/slash "); }
  return original(this, input);
}

static struct CommandInit {
  CommandInit() { Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), SlashCommand::setup); }
} xinit;

static ModuleRegister reg("ez:command", [](JsObjectWrapper native) -> std::string {
  native["executeCommand"] = +[](std::string const &command) {
    return ToJs(
        Mod::CommandSupport::GetInstance().ExecuteCommand(std::make_unique<Mod::CustomCommandOrigin>(), command));
  };
  native["setSlashCommandHandler"] = +[](JsValueRef handler) { holder = handler; };
  return R"js(
    export const executeCommand = import.meta.native.executeCommand;
    export const setSlashCommandHandler = import.meta.native.setSlashCommandHandler;
  )js";
});
