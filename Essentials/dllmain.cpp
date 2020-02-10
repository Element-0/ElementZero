// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <hook.h>

class TransferCommand : public Command {
  void execute(CommandOrigin const &, CommandOutput &) { std::wcout << "Executed" << std::endl; }
};

// TInstanceHook(void, ?registerOverloadInternal@CommandRegistry@@AEAAXAEAUSignature@1@AEAUOverload@1@@Z,
// CommandRegistry, Signature *signature, Overload *ovd) {
//  return original(this, signature, ovd);
//}

static void startRegister(CommandRegistry *registry) {
  std::wcout << L"int typid: " << Mod::CommandSupport::GetParameterTypeId<int>().value << std::endl;
  std::wcout << L"offset: " << offsetof(CommandRegistry::Overload, params) << std::endl;
  std::string name = "transferserver";
  registry->registerCommand(
      name, "commands.transferserver.description", CommandPermissionLevel::Normal, CommandFlagNone, CommandFlagNone);
  registry->registerOverload(name, &CommandRegistry::allocateCommand<TransferCommand>, {});
  std::wcout << "registered" << std::endl;
}

static void entry() { Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), startRegister); }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: entry(); break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH: break;
  }
  return TRUE;
}