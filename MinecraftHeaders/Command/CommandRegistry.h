#pragma once

#include "CommandFlag.h"
#include "CommandVersion.h"
#include "CommandPermissionLevel.h"
#include "../Core/type_id.h"

#include <vector>
#include <memory>
#include <string>
#include <string_view>

class Command;
class CommandOrigin;
class CommandParameterData;
class HardNonTerminal;

class CommandRegistry {
public:
#pragma region struct
  class ParseToken;
  struct Symbol {
    unsigned val;
  };
  struct Overload {
    using FactoryFn = std::unique_ptr<Command> (*)(void);
    CommandVersion version;                   // 0
    FactoryFn factory;                        // 8
    std::vector<CommandParameterData> params; // 16
    char unk;                                 // 40
    inline Overload(CommandVersion version, FactoryFn factory, std::initializer_list<CommandParameterData> args)
        : version(version), factory(factory), params(args), unk(0xFF) {}
  };
  struct Signature {
    std::string name;                                 // 0
    std::string desc;                                 // 32
    std::vector<CommandRegistry::Overload> overloads; // 64
    CommandPermissionLevel perm;                      // 88
    CommandRegistry::Symbol main_symbol;              // 92
    CommandRegistry::Symbol alt_symbol;               // 96
    CommandFlag flag;                                 // 100
    int unk72;
    int unk76;
    int unk80;
    bool b84;

    inline Signature(
        std::string_view name, std::string_view desc, CommandPermissionLevel perm, CommandRegistry::Symbol symbol,
        CommandFlag flag)
        : name(name), desc(desc), perm(perm), main_symbol(symbol), flag(flag) {}
  };

  struct SoftEnum {
    std::string name;              // 0
    std::vector<std::string> list; // 32
  };
#pragma endregion struct definition

  template <typename Type, typename TConverter>
  bool
  parseEnum(void *, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const {
    return true;
  }

  template <typename Type>
  bool parse(void *, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const;

  __declspec(dllimport) void registerCommand(
      std::string const &, char const *, CommandPermissionLevel, CommandFlag, CommandFlag);
  __declspec(dllimport) void registerAlias(std::string const &, std::string const &);

private:
  __declspec(dllimport) Signature const *findCommand(std::string const &) const;
  __declspec(dllimport) void registerOverloadInternal(Signature &, Overload &);

  __declspec(dllimport) Symbol addEnumValuesInternal(
      std::string const &, std::vector<std::pair<unsigned long, unsigned long>> const &, typeid_t<CommandRegistry>,
      bool (CommandRegistry::*)(
          void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &,
          std::vector<std::string> &) const);

  __declspec(dllimport) Symbol addEnumValuesInternal(
      std::string const &, std::vector<std::pair<std::string, unsigned long>> const &, typeid_t<CommandRegistry>,
      bool (CommandRegistry::*)(
          void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &,
          std::vector<std::string> &) const);
  unsigned addEnumValues(std::string const &, std::vector<std::string> const &);

public:
  template <typename T> inline static std::unique_ptr<Command> allocateCommand() { return std::make_unique<T>(); }
  inline void registerOverload(
      std::string const &name, Overload::FactoryFn factory, std::initializer_list<CommandParameterData> args) {
    Signature *signature = const_cast<Signature *>(findCommand(name));
    auto &overload       = signature->overloads.emplace_back(CommandVersion{}, factory, args);
    registerOverloadInternal(*signature, overload);
  }
};