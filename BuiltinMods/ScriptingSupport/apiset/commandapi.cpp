#include <cstddef>
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

static LeakedHolder holder;

static CommandOutputParameter ToCommandOutputParameter(JsValueRef val) {
  switch (GetJsType(val)) {
  case JsString: return {FromJs<std::string>(val)};
  case JsNumber: return {FromJs<double>(val)};
  default: return {JsToString(val)};
  }
}

static JsValueRef DumpCommandOrigin(CommandOrigin const &orig) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  JsObjectWrapper obj;
  obj["type"] = (int) orig.getOriginType();
  if (auto entity = orig.getEntity(); entity) {
    obj["entity"] = entity->getUniqueID();
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

static void WriteToCommandOutput(CommandOutput &outp, JsValueRef data) {
  switch (GetJsType(data)) {
  case JsUndefined:
  case JsNull: outp.success(); return;
  case JsArray: {
    JsObjectWrapper arr{data};
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
  case JsString: outp.success(FromJs<std::string>(data)); return;
  default: outp.success(JsToString(data)); return;
  }
}
static void WriteToCommandError(CommandOutput &outp, JsValueRef data) {
  switch (GetJsType(data)) {
  case JsUndefined:
  case JsNull: outp.error("unknown error"); return;
  case JsArray: {
    JsObjectWrapper arr{data};
    auto len = arr["length"].get<int>();
    switch (len) {
    case 0: outp.error("unknown error"); return;
    case 1: outp.error(JsToString(*arr[0])); return;
    default: {
      std::vector<CommandOutputParameter> params;
      for (int i = 1; i < len; i++) { params.emplace_back(ToCommandOutputParameter(*arr[i])); }
      outp.error(JsToString(*arr[0]), std::move(params));
      return;
    }
    }
    return;
  }
  case JsString: outp.error(FromJs<std::string>(data)); return;
  default: outp.error(JsToString(data)); return;
  }
}

static CommandRegistry *mCommandRegistry;

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
      WriteToCommandOutput(outp, ret);
    } catch (std::exception const &ex) { outp.error(ex.what()); } catch (Exception ref) {
      WriteToCommandError(outp, ref.raw);
    }
  }

  static void setup(CommandRegistry *registry) {
    mCommandRegistry = registry;
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

struct CustomCommand;

struct CommandDataDefinition {
  std::string name;
  size_t size{};
  CommandDataDefinition(std::string const &name, size_t size) : name(name), size(size) {}
  virtual ~CommandDataDefinition()                        = default;
  virtual void init(void *)                               = 0;
  virtual void deinit(void *)                             = 0;
  virtual JsValueRef fetch(void *, CommandOrigin const &) = 0;
  virtual CommandParameterData generate(size_t offset)    = 0;
};

template <typename T> struct BaseCommandDataDefinition : CommandDataDefinition {
  bool optional;

  BaseCommandDataDefinition(std::string const &name, bool optional)
      : CommandDataDefinition(name, optional ? sizeof(T) + sizeof(bool) : sizeof(T)), optional(optional) {}
  void init(void *x) override { new (x) T{}; }
  void deinit(void *x) override { std::destroy_at((T *) x); }
  JsValueRef fetch(void *x, CommandOrigin const &) override {
    if (optional) {
      auto is_set = *(bool *) ((char *) x + sizeof(T));
      if (!is_set) return GetUndefined();
    }
    return ToJs(*(T *) x);
  }
  CommandParameterData generate(size_t offset) override {
    return {
        Mod::CommandSupport::GetParameterTypeId<T>(),
        CommandRegistry::getParseFn<T>(),
        name,
        CommandParameterDataType::NORMAL,
        nullptr,
        (int) offset,
        optional,
        optional ? (int) (offset + sizeof(T)) : -1,
    };
  }
};

struct EnumCommandDataDefinition : BaseCommandDataDefinition<int> {
  inline static std::map<std::string, typeid_t<CommandRegistry>> tidmap;

  std::string enumname;
  typeid_t<CommandRegistry> id{0};

  EnumCommandDataDefinition(std::string const &name, std::string const &enumname, bool optional)
      : BaseCommandDataDefinition(name, optional), enumname(enumname) {
    if (auto it = tidmap.find(enumname); it != tidmap.end())
      id = it->second;
    else
      throw std::runtime_error{"enum not found"};
  }
  CommandParameterData generate(size_t offset) override {
    return {
        id,
        &CommandRegistry::fakeparse<int>,
        name,
        CommandParameterDataType::NORMAL,
        enumname.c_str(),
        (int) offset,
        optional,
        optional ? (int) (offset + sizeof(int)) : -1,
    };
  }
};

template <typename T> struct SelectorCommandDataDefinition : CommandDataDefinition {
  using Type = CommandSelector<T>;
  bool optional;

  SelectorCommandDataDefinition(std::string const &name, bool optional)
      : CommandDataDefinition(name, optional ? sizeof(Type) + sizeof(bool) : sizeof(Type)), optional(optional) {}

  void init(void *x) override { new (x) Type{}; }
  void deinit(void *x) override { std::destroy_at((Type *) x); }
  JsValueRef fetch(void *x, CommandOrigin const &orig) override {
    if (optional) {
      auto is_set = *(bool *) ((char *) x + sizeof(Type));
      if (!is_set) return GetUndefined();
    }
    Type &ref    = *(Type *) x;
    auto results = ref.results(orig);
    std::vector<JsValueRef> rets;
    auto &db = Mod::PlayerDatabase::GetInstance();
    for (auto &item : results) {
      if constexpr (std::is_same_v<T, Player>) {
        if (auto player = db.Find(item)) rets.emplace_back(ToJs(*player));
      } else {
        rets.emplace_back(ToJs(item->getUniqueID()));
      }
    }
    return ToJsArray(rets);
  }
  CommandParameterData generate(size_t offset) override {
    return {
        Mod::CommandSupport::GetParameterTypeId<Type>(),
        CommandRegistry::getParseFn<Type>(),
        name,
        CommandParameterDataType::NORMAL,
        nullptr,
        (int) offset,
        optional,
        optional ? (int) (offset + sizeof(T)) : -1,
    };
  }
};

struct CommandDataHolder {
  LeakedHolder func;
  std::vector<std::unique_ptr<CommandDataDefinition>> defs;

  CommandDataHolder(JsValueRef func) : func(func) {}
  void add(std::unique_ptr<CommandDataDefinition> def) { defs.emplace_back(std::move(def)); }
  std::vector<CommandParameterData> getCommandParameterData(size_t baseoff) {
    std::vector<CommandParameterData> ret;
    for (auto &def : defs) {
      ret.emplace_back(def->generate(baseoff));
      baseoff += def->size;
    }
    return ret;
  }
  size_t size() {
    size_t ret = 0;
    for (auto &def : defs) { ret += def->size; }
    return ret;
  }
  void init(void *start) {
    std::byte *offset = (std::byte *) start;
    for (auto &def : defs) {
      def->init(offset);
      offset += def->size;
    }
  }
  void deinit(void *start) {
    std::byte *offset = (std::byte *) start;
    for (auto &def : defs) {
      def->deinit(offset);
      offset += def->size;
    }
  }
  JsValueRef callFunction(void *start, JsValueRef self, CommandOrigin const &orig) {
    std::vector<JsValueRef> parr;
    parr.reserve(1 + defs.size());
    parr.emplace_back(self);
    auto offset = (std::byte *) start;
    for (auto &def : defs) {
      parr.emplace_back(def->fetch(offset, orig));
      offset += def->size;
    }
    JsValueRef result;
    ThrowError(JsCallFunction(*func, &parr[0], parr.size(), &result));
    return result;
  }
};

struct CustomCommand : Command {
  CommandDataHolder *holder;
  CustomCommand(CommandDataHolder *holder) : holder(holder) { holder->init((char *) this + sizeof(CustomCommand)); }
  ~CustomCommand() override { holder->deinit((char *) this + sizeof(CustomCommand)); }

  void execute(const CommandOrigin &orig, CommandOutput &outp) override {
    try {
      auto ret = holder->callFunction((char *) this + sizeof(CustomCommand), DumpCommandOrigin(orig), orig);
      WriteToCommandOutput(outp, ret);
    } catch (std::exception const &ex) { outp.error(ex.what()); } catch (Exception ref) {
      WriteToCommandError(outp, ref.raw);
    }
  }
};

#pragma pack(push, 1)
struct FuncPack {
  using FuncType      = std::unique_ptr<Command> (*)();
  const short mov_r10 = 0xba49;
  const CommandDataHolder *holder;
  const short mov_rax = 0xb848;
  const void *ptr     = (void *) wrapper;
  const short jmp     = 0xe0ff;
  FuncPack(CommandDataHolder *holder) : holder{holder} {}

  static std::unique_ptr<Command> wrapper() {
    CommandDataHolder *holder;
    asm("movq %%r10, %0" : "=r"(holder));
    auto buffer = new char[sizeof(CustomCommand) + holder->size()];
    new (buffer) CustomCommand(holder);
    return std::unique_ptr<Command>((Command *) buffer);
  }

  static FuncType create(CommandDataHolder *holder) {
    auto mem = VirtualAlloc(nullptr, sizeof(FuncPack), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    new (mem) FuncPack(holder);
    return (FuncType) mem;
  }
};
#pragma pack(pop)

static CommandDataHolder *GenCommandDataHolder(JsValueRef arr, JsValueRef func) {
  std::unique_ptr<CommandDataHolder> holder = std::make_unique<CommandDataHolder>(func);
  JsObjectWrapper args{arr};
  auto len = args["length"].get<int>();
  for (int i = 0; i < len; i++) {
    auto tmp = args[i];
    if (tmp.type() != JsObject) throw std::runtime_error{"type mismatch"};
    JsObjectWrapper arg{*tmp};
    auto type = arg["type"].get<std::string>();
    auto name = arg["name"].get<std::string>();
    auto opt  = arg["optional"].get<bool>();
    if (type == "bool") {
      holder->add(std::make_unique<BaseCommandDataDefinition<bool>>(name, opt));
    } else if (type == "int") {
      holder->add(std::make_unique<BaseCommandDataDefinition<int>>(name, opt));
    } else if (type == "float") {
      holder->add(std::make_unique<BaseCommandDataDefinition<float>>(name, opt));
    } else if (type == "string") {
      holder->add(std::make_unique<BaseCommandDataDefinition<std::string>>(name, opt));
    } else if (type == "json") {
      holder->add(std::make_unique<BaseCommandDataDefinition<Json::Value>>(name, opt));
    } else if (type == "enum") {
      auto enumname = arg["enum"].get<std::string>();
      holder->add(std::make_unique<EnumCommandDataDefinition>(name, enumname, opt));
    } else if (type == "players") {
      holder->add(std::make_unique<SelectorCommandDataDefinition<Player>>(name, opt));
    } else if (type == "entities") {
      holder->add(std::make_unique<SelectorCommandDataDefinition<Actor>>(name, opt));
    } else
      throw std::runtime_error{"unsupported type: " + type};
  }
  return holder.release();
}

static ModuleRegister reg("ez:command", [](JsObjectWrapper native) -> std::string {
  native["executeCommand"] = +[](std::string const &command) {
    return ToJs(
        Mod::CommandSupport::GetInstance().ExecuteCommand(std::make_unique<Mod::CustomCommandOrigin>(), command));
  };
  native["setSlashCommandHandler"] = +[](JsValueRef handler) { holder = handler; };
  native["registerCommand"]        = +[](std::string const &name, std::string const &desc, int permission) {
    mCommandRegistry->registerCommand(
        name, desc.c_str(), (CommandPermissionLevel) permission, CommandFlagCheat, CommandFlagNone);
  };
  native["registerAlias"] =
      +[](std::string const &name, std::string const &altname) { mCommandRegistry->registerAlias(name, altname); };
  native["registerOverride"] = +[](std::string const &name, JsValueRef arr, JsValueRef func) {
    auto holder = GenCommandDataHolder(arr, func);
    mCommandRegistry->registerOverload(
        name, FuncPack::create(holder), holder->getCommandParameterData(sizeof(CustomCommand)));
  };
  native["addEnum"] = +[](std::string const &name, Json::Value arr) {
    if (!arr.isArray()) throw std::runtime_error{"require array as argument"};
    std::vector<std::pair<std::string, int>> parsed;
    int i = 0;
    for (auto &item : arr) {
      if (!item.isString()) throw std::runtime_error{"require string[]"};
      parsed.emplace_back(item.asString("unk"), i++);
    }
    auto id = commands::addCustomEnum(mCommandRegistry, name.c_str(), parsed);
    EnumCommandDataDefinition::tidmap.emplace(name, id);
  };
  return R"js(
    export const executeCommand = import.meta.native.executeCommand;
    export const setSlashCommandHandler = import.meta.native.setSlashCommandHandler;
    export const registerAlias = import.meta.native.registerAlias;
    export const registerCommand = import.meta.native.registerCommand;
    export const registerOverride = import.meta.native.registerOverride;
    export const addEnum = import.meta.native.addEnum;
  )js";
});
