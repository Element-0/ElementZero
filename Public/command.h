#pragma once

#include <string>
#include <string_view>

#include "event.h"
#include <Core/type_id.h>
#include <Command/CommandSelector.h>
#include <Command/CommandParameterData.h>

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

namespace commands {

template <typename Command, typename Type> int getOffset(Type Command::*src) {
  union {
    Type Command::*src;
    int value;
  } u;
  u.src = src;
  return u.value;
}

template <typename Command, typename Type>
CommandParameterData mandatory(Type Command::*field, std::string_view name, bool Command::*isSet = nullptr) {
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
template <typename Command, typename Type>
CommandParameterData optional(Type Command::*field, std::string_view name, bool Command::*isSet = nullptr) {
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

} // namespace commands

namespace Mod {

class CommandSupport : public EventEmitter<"loaded"_sig, CommandRegistry *> {
  COMMANDAPI CommandSupport();

public:
  USING_EVENTEMITTER("loaded", CommandRegistry *);
  COMMANDAPI static CommandSupport &GetInstance();

  template <typename T> static typeid_t<CommandRegistry> GetParameterTypeId();
};

template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<bool>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<int>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<float>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<std::string>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<CommandItem>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<Json::Value>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<CommandSelector<Actor>>();
template <> COMMANDAPI typeid_t<CommandRegistry> CommandSupport::GetParameterTypeId<CommandSelector<Player>>();

} // namespace Mod