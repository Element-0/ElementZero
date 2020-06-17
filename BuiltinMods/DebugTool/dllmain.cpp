#include <vector>

#include <Command/CommandRegistry.h>
#include <Command/CommandParameterData.h>
#include <dllentry.h>
#include <boost/format.hpp>
#include <iostream>
#include <hook.h>
#include <base/log.h>

DEF_LOGGER("debug");

static struct Settings {
  struct Command {
    bool logRegister = false;
    bool enableAll   = false;

    template <typename IO> static inline bool io(IO f, Command &command, YAML::Node &node) {
      return f(command.logRegister, node["log-register"]) && f(command.enableAll, node["enable-all"]);
    }
  } command;

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.command, node["command"]);
  }
} settings;

DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

TInstanceHook(
    void, "?registerOverloadInternal@CommandRegistry@@AEAAXAEAUSignature@1@AEAUOverload@1@@Z", CommandRegistry,
    Signature &signature, Overload &ovd) {
  if (settings.command.logRegister) {
    DEF_LOGGER("CommandRegister");
    LOGV("override");
    for (auto &param : ovd.params) {
      union {
        void *pointer;
        decltype(param.parser) parser;
      } u;
      u.parser = param.parser;
      LOGV("\t%c %s[%s] %d(%d) +%d(%d) @%p") % (param.optional ? '+' : '-') % param.name %
          (param.desc ? param.desc : "") % param.tid.value % (int) param.type % param.offset % param.flag_offset %
          u.pointer;
      if (param.unk56 != -1 || param.pad73) LOGW("\t\tunk56 %d pad73 %d") % param.unk56 % param.pad73;
    }
  }
  return original(this, signature, ovd);
}

TInstanceHook(
    void,
    "?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@"
    "PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
    CommandRegistry, std::string const &name, char const *desc, CommandPermissionLevel perm, CommandFlag a,
    CommandFlag b) {
  if (settings.command.enableAll) {
    if (a != CommandFlagCheat && b != CommandFlagCheat) { b |= CommandFlagCheat; }
    if (a == CommandFlagVisibility6) { a = CommandFlagNone; }
  }
  if (settings.command.logRegister) {
    DEF_LOGGER("CommandRegister");
    LOGV("Command: %s (%s) [%d/%d] {%d}") % name % desc % (int) a.value % (int) b.value % (int) perm;
  }
  return original(this, name, desc, perm, a, b);
}

TInstanceHook(
    void,
    "?addEnumValueConstraints@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@"
    "AEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_"
    "traits@D@std@@V?$allocator@D@2@@std@@@2@@3@W4SemanticConstraint@@@Z",
    CommandRegistry, std::string const &en, std::vector<std::string> const &data, int value) {
  if (settings.command.logRegister) {
    DEF_LOGGER("CommandRegister");
    LOGV("Enum constraints for %s: (%d)") % en % value;
    for (auto &it : data) { LOGV("\t%s") % it; }
  }
  original(this, en, data, value);
}

TInstanceHook2(
    "CommandRegistry::addEnumValuesInternal", CommandRegistry::Symbol,
    "?addEnumValuesInternal@CommandRegistry@@AEAA?AVSymbol@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@"
    "2@@std@@AEBV?$vector@U?$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_K@std@@V?$allocator@U?"
    "$pair@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_K@std@@@2@@4@V?$typeid_t@VCommandRegistry@@@@"
    "P81@EBA_NPEAXAEBUParseToken@1@AEBVCommandOrigin@@HAEAV34@AEAV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$"
    "allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@4@@Z@Z",
    CommandRegistry, std::string const &name, std::vector<std::pair<std::string, uint64_t>> const &mapped,
    typeid_t<CommandRegistry> tid,
    bool (CommandRegistry::*parser)(
        void *, CommandRegistry::ParseToken const &, CommandOrigin const &, int, std::string &,
        std::vector<std::string> &) const) {
  if (settings.command.logRegister) {
    DEF_LOGGER("CommandRegister");
    LOGV("Enum: %s [%d]") % name % tid.value;
    for (auto &[k, v] : mapped) LOGV("\t%s -> %d") % k % v;
  }
  return original(this, name, mapped, tid, parser);
}