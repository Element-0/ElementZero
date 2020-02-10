// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <command.h>

void (Mod::CommandSupport::*emitter)(sigt<"loaded"_sig>, CommandRegistry *);

Mod::CommandSupport::CommandSupport() { emitter = &Mod::CommandSupport::Emit; }

Mod::CommandSupport &Mod::CommandSupport::GetInstance() {
  static CommandSupport instance;
  return instance;
}

THook(void, ?setup@XPCommand@@SAXAEAVCommandRegistry@@@Z, CommandRegistry *registry) {
  (Mod::CommandSupport::GetInstance().*emitter)(SIG("loaded"), registry);
  original(registry);
}

template <std::uint32_t> inline static typeid_t<CommandRegistry> getid(char const *name) {
  static auto ptr = (typeid_t<CommandRegistry> *) GetServerSymbol(name);
  return *ptr;
}

#define GETID(name) getid<name##_sig>(name)

template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<bool>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@_N@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<int>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@H@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<float>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@M@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<std::string>() {
  return GETID(
      "?id@?1???$type_id@VCommandRegistry@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@@YA?AV?$"
      "typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<CommandItem>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@VCommandItem@@@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<Json::Value>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@VValue@Json@@@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH: break;
  }
  return TRUE;
}