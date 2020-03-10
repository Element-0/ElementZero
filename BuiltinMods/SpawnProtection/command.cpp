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

class SpawnProtectionCommand : public Command {
public:
  Mode target_mode;
  SpawnProtectionCommand() {}

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    mode = target_mode;
    output.success("commands.spawn-protection.success", {dumpMode(mode)});
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    addEnum<Mode>(
        registry, "spawn-protection-mode",
        {{"disable", Mode::Disabled}, {"enforce", Mode::Enforce}, {"permissive", Mode::Permissive}});
    registry->registerCommand(
        "spawn-protection", "commands.spawn-protection.description", CommandPermissionLevel::GameMasters,
        CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<SpawnProtectionCommand>(
        "spawn-protection", mandatory<CommandParameterDataType::ENUM>(
                                &SpawnProtectionCommand::target_mode, "mode", "spawn-protection-mode"));
  }
};

void initCommand(CommandRegistry *registry) { SpawnProtectionCommand::setup(registry); }