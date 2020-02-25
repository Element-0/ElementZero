#include <Command/CommandRegistry.h>
#include <Command/CommandParameterData.h>
#include <dllentry.h>
#include <boost/format.hpp>
#include <iostream>
#include <hook.h>
#include <log.h>

DEF_LOGGER("debug");

static struct Settings {
  struct Command {
    bool logRegister = false;

    template <typename IO> static inline bool io(IO f, Command &command, YAML::Node &node) {
      return f(command.logRegister, node["log-register"]);
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
    LOGV("Command: %s (%s) [%d] {%d}") % signature.name % signature.desc % (int) signature.flag.value %
        (int) signature.perm;
    for (auto &param : ovd.params) {
      LOGV("\t%s[%s] %d(%d) +%d(%d)") % param.name % (param.desc ? param.desc : "") % param.tid.value %
          (int) param.type % param.offset % param.flag_offset;
    }
  }
  return original(this, signature, ovd);
}