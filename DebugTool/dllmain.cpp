#include <Command/CommandRegistry.h>
#include <Command/CommandParameterData.h>
#include <dllentry.h>
#include <boost/format.hpp>
#include <iostream>
#include <hook.h>

static struct Settings {
  struct Command {
    bool logRegister = false;
  } command;
} settings;

void ApplySettings(YAML::Node const &node) {
  if (auto subcmd = node["command"]; subcmd) { settings.command.logRegister = subcmd["log-register"].as<bool>(false); }
}

void dllenter() {}
void dllexit() {}

TInstanceHook(
    void, "?registerOverloadInternal@CommandRegistry@@AEAAXAEAUSignature@1@AEAUOverload@1@@Z", CommandRegistry,
    Signature &signature, Overload &ovd) {
  if (settings.command.logRegister) {
    std::cout << boost::format("Command: %s (%s) [%d] {%d}") % signature.name % signature.desc %
                     (int) signature.flag.value % (int) signature.perm
              << std::endl;
    for (auto &param : ovd.params) {
      std::cout << boost::format("\t%s[%s] %d(%d) +%d(%d)") % param.name % (param.desc ? param.desc : "") %
                       param.tid.value % (int) param.type % param.offset % param.flag_offset
                << std::endl;
    }
  }
  return original(this, signature, ovd);
}