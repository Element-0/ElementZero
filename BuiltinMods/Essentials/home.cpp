#include "pch.h"

#include "global.h"

class HomeCommand : public Command {
public:
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {"/home"});
      return;
    }
    auto source = (Player *) origin.getEntity();
    auto pos    = source->getSpawnPosition();
    if (pos.y < 0) {
      output.error("commands.home.error");
      return;
    }
    source->teleport(pos, {0}, 0);
    output.success("commands.home.success");
  }
  static void setup(CommandRegistry *registry) {
    registry->registerCommand(
        "home", "commands.home.description", CommandPermissionLevel::Normal, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<HomeCommand>("home");
  }
};

void registerHome(CommandRegistry *registry) { HomeCommand::setup(registry); }