#include <Command/Command.h>
#include <Command/CommandOrigin.h>
#include <Command/CommandOutput.h>
#include <Command/CommandSelector.h>
#include <Command/CommandSelectorResults.h>
#include <Command/CommandPermissionLevel.h>

#include <economy.h>
#include <command.h>

#include "global.h"

class BalanceCommand : public Command {
public:
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
      output.error("commands.generic.error.invalidPlayer", {"/balance"});
      return;
    }
    auto player  = (Player *) origin.getEntity();
    auto balance = Mod::Economy::GetBalance(player);
    output.success("commands.balance.success", {balance});
  }
  static void setup(CommandRegistry *registry) {
    registry->registerCommand(
        "balance", "commands.balance.description", CommandPermissionLevel::Any, CommandFlagCheat, CommandFlagNone);
    registry->registerOverload<BalanceCommand>("balance");
  }
};

class UpdateBalanceCommand : public Command {
  CommandSelector<Player> selector;
  int amount;
  std::string reason;

public:
  UpdateBalanceCommand() { selector.setIncludeDeadPlayers(true); }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    auto result = selector.results(origin);
    if (result.empty()) {
      output.error("commands.generic.selector.empty");
      return;
    }
    {
      Mod::Economy::Transaction trans;
      for (auto target : result) {
        auto old = Mod::Economy::GetBalance(target);
        auto err = Mod::Economy::UpdateBalance(target, amount, reason);
        if (err) {
          output.error("commands.update-balance.error", {(Actor const *) target, *err});
          return;
        }
        output.success("commands.update-balance.entry", {(Actor const *) target, amount, old + amount});
      }
      trans.Commit();
      output.success("commands.update-balance.success");
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "update-balance", "commands.update-balance.description", CommandPermissionLevel::GameMasters, CommandFlagCheat,
        CommandFlagNone);
    registry->registerOverload<UpdateBalanceCommand>(
        "update-balance", mandatory(&UpdateBalanceCommand::selector, "target"),
        mandatory(&UpdateBalanceCommand::amount, "amount"), mandatory(&UpdateBalanceCommand::reason, "reason"));
  }
};

class GetBalanceCommand : public Command {
  CommandSelector<Player> selector;

public:
  GetBalanceCommand() { selector.setIncludeDeadPlayers(true); }

  void execute(CommandOrigin const &origin, CommandOutput &output) {
    auto result = selector.results(origin);
    if (result.empty()) {
      output.error("commands.generic.selector.empty");
      return;
    }
    for (auto target : result) {
      auto balance = Mod::Economy::GetBalance(target);
      output.success("commands.get-balance.success", {(Actor const *) target, balance});
    }
  }
  static void setup(CommandRegistry *registry) {
    using namespace commands;
    registry->registerCommand(
        "get-balance", "commands.get-balance.description", CommandPermissionLevel::GameMasters, CommandFlagCheat,
        CommandFlagNone);
    registry->registerOverload<GetBalanceCommand>("get-balance", mandatory(&GetBalanceCommand::selector, "target"));
  }
};

void InitCommand(CommandRegistry *registry) {
  BalanceCommand::setup(registry);
  UpdateBalanceCommand::setup(registry);
  GetBalanceCommand::setup(registry);
}