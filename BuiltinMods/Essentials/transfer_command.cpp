#include "pch.h"

#include "global.h"

class TransferCommand : public Command {
public:
  CommandSelector<Player> selector;
  std::string hostname = "127.0.0.1";
  int port             = 19132;
  TransferCommand() { selector.setIncludeDeadPlayers(true); }
  void execute(CommandOrigin const &origin, CommandOutput &output) {
    if (port <= 0 || port > 65535) {
      output.error("commands.transferserver.invalid.port");
      return;
    }
    auto results = selector.results(origin);
    for (auto &player : results) {
      TransferPacket pkt{hostname, port};
      player->sendNetworkPacket(pkt);
    }
    output.success("commands.transferserver.successful");
  }
};

void registerTransferServer(CommandRegistry *registry) {
  using namespace commands;
  std::string name = "transferserver";
  registry->registerCommand(
      name, "commands.transferserver.description", CommandPermissionLevel::Privileged, CommandFlagNone,
      CommandFlagNone);
  registry->registerOverload<TransferCommand>(
      name, mandatory(&TransferCommand::selector, "target"), mandatory(&TransferCommand::hostname, "hostname"),
      optional(&TransferCommand::port, "port"));
}
