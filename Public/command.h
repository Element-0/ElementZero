#pragma once

#include <string>

#include "event.h"
#include <Core/type_id.h>

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

} // namespace Mod