#include "pch.h"

// Working in progress
class TransferCommand : public Command {
  void execute(CommandOrigin const &, CommandOutput &) { std::wcout << "Executed (WIP)" << std::endl; }
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
    rhs.transferserver = node["transferserver"].as<bool>(true);
    return true;
  }
};
template <> struct convert<Settings> {
  static bool decode(const Node &node, Settings &rhs) {
    if (auto commands = node["commands"]; commands) rhs.commands = commands.as<Settings::Commands>();
    return true;
  }
};
} // namespace YAML

extern "C" __declspec(dllexport) void ApplySettings(YAML::Node const &node) { settings = node.as<Settings>(); }

static void startRegister(CommandRegistry *registry) {
  if (settings.commands.transferserver) {
    std::string name = "transferserver";
    registry->registerCommand(
        name, "commands.transferserver.description", CommandPermissionLevel::Normal, CommandFlagNone, CommandFlagNone);
    registry->registerOverload(name, &CommandRegistry::allocateCommand<TransferCommand>, {});
  }
}

void dllenter() { Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), startRegister); }
void dllexit() {}