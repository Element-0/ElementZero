#include "pch.h"

TClasslessInstanceHook(
  int,
  ?start@DedicatedServer@@QEAA?AW4StartResult@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z,
  void *name) {
  std::cout << "hooked: " << (void *) name << std::endl;
  return original(this, name);
}

static void entry() { std::cout << "entry" << std::endl; }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: entry(); break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH: break;
  }
  return TRUE;
}
