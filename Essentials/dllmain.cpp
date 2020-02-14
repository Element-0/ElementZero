#include "pch.h"

// Working in progress
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

static struct Settings {
  struct Commands {
    bool transferserver = true;
  } commands;
} settings;

static_assert(sizeof(std::string) == 32);

namespace YAML {
template <> struct convert<Settings::Commands> {
  static bool decode(const Node &node, Settings::Commands &rhs) {
    yaml_assign(rhs.transferserver, node["transferserver"]);
    return true;
  }
};
template <> struct convert<Settings> {
  static bool decode(const Node &node, Settings &rhs) {
    if (auto commands = node["commands"]; commands) yaml_assign(rhs.commands, commands);
    return true;
  }
};
} // namespace YAML

extern "C" __declspec(dllexport) void ApplySettings(YAML::Node const &node) { yaml_assign(settings, node); }

static void startRegister(CommandRegistry *registry) {
  using namespace commands;
  if (settings.commands.transferserver) {
    std::string name = "transferserver";
    registry->registerCommand(
        name, "commands.transferserver.description", CommandPermissionLevel::Privileged, CommandFlagNone,
        CommandFlagNone);
    registry->registerOverload<TransferCommand>(
        name, mandatory(&TransferCommand::selector, "target"), mandatory(&TransferCommand::hostname, "hostname"),
        optional(&TransferCommand::port, "port"));
  }
}

void dllenter() { Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), startRegister); }
void dllexit() {}