#include <Command/CommandRegistry.h>
#include <Command/CommandParameterData.h>
#include <dllentry.h>
#include <boost/format.hpp>
#include <iostream>
#include <hook.h>
#include <log.h>

static struct Settings {
  struct Command {
    bool logRegister = false;
  } command;
} settings;

void ApplySettings(YAML::Node const &node) {
  if (auto subcmd = node["command"]; subcmd) { yaml_assign(settings.command.logRegister, subcmd["log-register"]); }
}

void dllenter() {}
void dllexit() {}

TInstanceHook(
    void, "?registerOverloadInternal@CommandRegistry@@AEAAXAEAUSignature@1@AEAUOverload@1@@Z", CommandRegistry,
    Signature &signature, Overload &ovd) {
  if (settings.command.logRegister) {
    DEF_LOGGER("CommandRegister");
    LOGV("Command: %s (%s) [%d] {%d}") % signature.name % signature.desc % (int) signature.flag.value %
        (int) signature.perm;
    for (auto &param : ovd.params) {
      LOGV("\t%s[%s] %d(%d) +%d(%d)") % param.name % (param.desc ? param.desc : "") % param.tid.value %
          (int) param.type % param.offset % param.flag_offset;
    }
  }
  return original(this, signature, ovd);
}