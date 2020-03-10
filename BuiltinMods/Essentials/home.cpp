#include "pch.h"

#include "global.h"

#include <Level/LevelData.h>

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
        "home", "commands.home.description", CommandPermissionLevel::Any, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<HomeCommand>("home");
  }
};

class SpawnCommand : public Command {
public:
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {"/spawn"});
      return;
    }
    auto &level = LocateService<Level>()->GetLevelDataWrapper();
    if (!level->mHasSpawnPos) {
      output.error("commands.spawn.error.not.set");
      return;
    }
    auto source = (Player *) origin.getEntity();
    auto pos    = level->mSpawnPos;
    source->teleport(pos, {0}, 0);
    output.success("commands.spawn.success");
  }
  static void setup(CommandRegistry *registry) {
    registry->registerCommand(
        "spawn", "commands.spawn.description", CommandPermissionLevel::Any, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<SpawnCommand>("spawn");
  }
};

void registerHome(CommandRegistry *registry) {
  HomeCommand::setup(registry);
  SpawnCommand::setup(registry);
}