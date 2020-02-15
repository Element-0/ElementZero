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
struct CommandParameterData;
class HardNonTerminal;

class CommandRegistry {
public:
#pragma region struct
  struct ParseToken;
  class Symbol {
  public:
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

  __declspec(dllimport) void registerCommand(
      std::string const &, char const *, CommandPermissionLevel, CommandFlag, CommandFlag);
  __declspec(dllimport) void registerAlias(std::string const &, std::string const &);

private:
  __declspec(dllimport) Signature const *findCommand(std::string const &) const;
  __declspec(dllimport) void registerOverloadInternal(Signature &, Overload &);

  template <typename Type>
  __declspec(dllimport) bool parse(
      void *, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const;

  __declspec(dllimport) Symbol addEnumValuesInternal(
      std::string const &, std::vector<std::pair<uint64_t, uint64_t>> const &, typeid_t<CommandRegistry>,
      bool (CommandRegistry::*)(
          void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &,
          std::vector<std::string> &) const);

  __declspec(dllimport) Symbol addEnumValuesInternal(
      std::string const &, std::vector<std::pair<std::string, uint64_t>> const &, typeid_t<CommandRegistry>,
      bool (CommandRegistry::*)(
          void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &,
          std::vector<std::string> &) const);
  __declspec(dllimport) unsigned addEnumValues(std::string const &, std::vector<std::string> const &);
  __declspec(dllimport) uint64_t getEnumData(CommandRegistry::ParseToken const &) const;

public:
  template <typename T> inline static auto getParseFn() { return &CommandRegistry::parse<T>; }

  template <typename Type>
  bool
  fakeparse(void *, ParseToken const &, CommandOrigin const &, int, std::string &, std::vector<std::string> &) const {
    return false;
  }

  template <typename Type> struct DefaultIdConverter {
    template <typename Target, typename Source> static Target convert(Source source) { return (Target) source; }
    uint64_t operator()(Type value) const { return convert<uint64_t, Type>(value); }
    Type operator()(uint64_t value) const { return convert<Type, uint64_t>(value); }
  };

  template <typename Type, typename IDConverter = CommandRegistry::DefaultIdConverter<Type>>
  bool parseEnum(
      void *target, CommandRegistry::ParseToken const &token, CommandOrigin const &, int, std::string &,
      std::vector<std::string> &) const {
    auto data        = getEnumData(token);
    *(Type *) target = IDConverter{}(data);
    return true;
  }

  template <typename Type, typename IDConverter = CommandRegistry::DefaultIdConverter<Type>>
  unsigned addEnumValues(
      std::string const &name, typeid_t<CommandRegistry> tid,
      std::initializer_list<std::pair<std::string, Type>> const &values) {
    std::vector<std::pair<std::string, uint64_t>> converted;
    IDConverter converter;
    for (auto &value : values) converted.emplace_back(value.first, converter(value.second));
    return addEnumValuesInternal(name, converted, tid, &CommandRegistry::parseEnum<Type, IDConverter>).val;
  }

  template <typename T> inline static std::unique_ptr<Command> allocateCommand() { return std::make_unique<T>(); }
  inline void registerOverload(
      std::string const &name, Overload::FactoryFn factory, std::initializer_list<CommandParameterData> args) {
    Signature *signature = const_cast<Signature *>(findCommand(name));
    auto &overload       = signature->overloads.emplace_back(CommandVersion{}, factory, args);
    registerOverloadInternal(*signature, overload);
  }
  template <typename T, typename... Params> inline void registerOverload(std::string const &name, Params... params) {
    registerOverload(name, &allocateCommand<T>, {params...});
  }
};