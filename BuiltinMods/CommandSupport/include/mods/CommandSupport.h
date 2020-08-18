#pragma once

#include <memory>
#include <string>
#include <type_traits>

#include <Core/type_id.h>
#include <Core/Util.h>
#include <Core/json.h>
#include <Core/RelativeFloat.h>
#include <Core/MCRESULT.h>
#include <Command/MinecraftCommands.h>
#include <Command/Command.h>
#include <Command/CommandRawText.h>
#include <Command/CommandOutput.h>
#include <Command/CommandContext.h>
#include <Command/CommandRegistry.h>
#include <Command/CommandOrigin.h>
#include <Command/CommandSelector.h>
#include <Command/CommandRegistry.h>
#include <Command/CommandParameterData.h>

#include <base/base.h>
#include <base/event.h>

#ifdef CommandSupport_EXPORTS
#  define COMMANDAPI __declspec(dllexport)
#else
#  define COMMANDAPI __declspec(dllimport)
#endif

class CommandRegistry;
class CommandItem;
namespace Json {
class Value;
}

namespace Mod {

class CustomCommandOrigin;

// Custom command registration related functions
class CommandSupport : public EventEmitter<"loaded"_sig, CommandRegistry *> {
  COMMANDAPI CommandSupport();
  static COMMANDAPI short &type_id_count();

public:
  USING_EVENTEMITTER("loaded", CommandRegistry *);
  COMMANDAPI static CommandSupport &GetInstance();

  // Execute command and get the result
  COMMANDAPI Json::Value ExecuteCommand(std::unique_ptr<CustomCommandOrigin> origin, std::string command);

  template <typename T> static typeid_t<CommandRegistry> GetParameterTypeId() {
    static auto value = type_id_count()++;
    return typeid_t<CommandRegistry>{value};
  }
  static typeid_t<CommandRegistry> GetCustomParameterTypeId() { return typeid_t<CommandRegistry>{type_id_count()++}; }
};

template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<bool>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<int>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<float>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<RelativeFloat>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<std::string>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<CommandItem>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<Json::Value>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<CommandSelector<Actor>>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<CommandSelector<Player>>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<CommandRawText>();

class CustomCommandOrigin : public CommandOrigin {
public:
  std::string name;
  BlockPos pos                 = {0, 0, 0};
  Vec3 worldPosition           = {0, 0, 0};
  Dimension *dim               = nullptr;
  Actor *actor                 = nullptr;
  CommandPermissionLevel level = CommandPermissionLevel::Internal;
  CommandOriginType type       = CommandOriginType::Script;
  bool allowSelectorExpansion  = true;
  Json::Value *result          = nullptr;

  inline CustomCommandOrigin() {}
  static std::unique_ptr<CustomCommandOrigin> CopyFrom(CommandOrigin const &orig) {
    auto ret           = std::make_unique<CustomCommandOrigin>();
    ret->pos           = orig.getBlockPosition();
    ret->worldPosition = orig.getWorldPosition();
    ret->dim           = orig.getDimension();
    ret->actor         = orig.getEntity();
    return ret;
  }

  std::string const &getRequestId() const override { return Util::EMPTY_GUID; }
  std::string getName() const override { return name; }
  BlockPos getBlockPosition() const override { return pos; }
  Vec3 getWorldPosition() const override { return worldPosition; }
  Level *getLevel() const override { return LocateService<Level>(); }
  Dimension *getDimension() const override { return dim; }
  Actor *getEntity() const override { return actor; }
  CommandPermissionLevel getPermissionsLevel() const override { return level; }
  std::unique_ptr<CustomCommandOrigin> custom_clone(Json::Value *new_result = nullptr) const {
    return std::make_unique<CustomCommandOrigin>(*this);
  }
  std::unique_ptr<CommandOrigin> clone() const override { return custom_clone(); }
  CommandOriginType getOriginType() const override { return type; }
  bool canUseCommandsWithoutCheatsEnabled() const override { return true; }
  bool isSelectorExpansionAllowed() const override { return allowSelectorExpansion; }
  void handleCommandOutputCallback(Json::Value &&val) const override {
    if (result) *result = std::move(val);
  }
};

} // namespace Mod

namespace commands {

template <typename T>
char const *addEnum(CommandRegistry *registry, char const *name, std::vector<std::pair<std::string, T>> const &values) {
  registry->addEnumValues<T>(name, Mod::CommandSupport::GetParameterTypeId<T>(), values);
  return name;
}

inline typeid_t<class CommandRegistry>
addCustomEnum(CommandRegistry *registry, char const *name, std::vector<std::pair<std::string, int>> const &values) {
  auto id = Mod::CommandSupport::GetCustomParameterTypeId();
  registry->addEnumValues<int>(name, id, values);
  return id;
}

template <typename Command, typename Type> int getOffset(Type Command::*src) {
  union {
    Type Command::*src;
    int value;
  } u;
  u.src = src;
  return u.value;
}

template <typename Command, typename Type>
CommandParameterData mandatory(Type Command::*field, std::string name, bool Command::*isSet = nullptr) {
  return {
      Mod::CommandSupport::GetParameterTypeId<Type>(),
      CommandRegistry::getParseFn<Type>(),
      name,
      CommandParameterDataType::NORMAL,
      nullptr,
      getOffset(field),
      false,
      isSet ? getOffset(isSet) : -1,
  };
}
template <CommandParameterDataType DataType, typename Command, typename Type>
CommandParameterData
mandatory(Type Command::*field, std::string name, char const *desc = nullptr, bool Command::*isSet = nullptr) {
  return {
      Mod::CommandSupport::GetParameterTypeId<Type>(),
      &CommandRegistry::fakeparse<Type>,
      name,
      DataType,
      desc,
      getOffset(field),
      false,
      isSet ? getOffset(isSet) : -1,
  };
}
template <typename Command, typename Type>
CommandParameterData optional(Type Command::*field, std::string name, bool Command::*isSet = nullptr) {
  return {
      Mod::CommandSupport::GetParameterTypeId<Type>(),
      CommandRegistry::getParseFn<Type>(),
      name,
      CommandParameterDataType::NORMAL,
      nullptr,
      getOffset(field),
      true,
      isSet ? getOffset(isSet) : -1,
  };
}
template <CommandParameterDataType DataType, typename Command, typename Type>
CommandParameterData
optional(Type Command::*field, std::string name, char const *desc = nullptr, bool Command::*isSet = nullptr) {
  return {
      Mod::CommandSupport::GetParameterTypeId<Type>(),
      &CommandRegistry::fakeparse<Type>,
      name,
      DataType,
      desc,
      getOffset(field),
      true,
      isSet ? getOffset(isSet) : -1,
  };
}

} // namespace commands