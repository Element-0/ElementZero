#include <command.h>
#include <Command/Command.h>
#include <Command/CommandOrigin.h>
#include <Command/CommandOutput.h>
#include <Command/CommandRegistry.h>

#include "settings.h"

static char const *dumpMode(Mode mode) {
  switch (mode) {
  case Mode::Disabled: return "disabled";
  case Mode::Enforce: return "enforce";
  case Mode::Permissive: return "permissive";
  default: return "unknown";
  }
}

class CoreProtectCommand : public Command {
public:
  Mode target_mode;
  CoreProtectCommand() {}

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    mode = target_mode;
    output.success("commands.core-protect.success", {dumpMode(mode)});
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    addEnum<Mode>(
        registry, "core-protect-mode",
        {{"disable", Mode::Disabled}, {"enforce", Mode::Enforce}, {"permissive", Mode::Permissive}});
    registry->registerCommand(
        "core-protect", "commands.core-protect.description", CommandPermissionLevel::Privileged, CommandFlagCheat,
        CommandFlagNone);
    registry->registerOverload<CoreProtectCommand>(
        "core-protect",
        mandatory<CommandParameterDataType::ENUM>(&CoreProtectCommand::target_mode, "mode", "core-protect-mode"));
  }
};

void initCommand(CommandRegistry *registry) { CoreProtectCommand::setup(registry); }