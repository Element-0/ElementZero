#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "yaml.h"

void dllenter();
void dllexit();

extern "C" __declspec(dllexport) void ApplySettings(YAML::Node const &node);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: dllenter(); break;
  case DLL_THREAD_ATTACH: break;
  case DLL_THREAD_DETACH: break;
  case DLL_PROCESS_DETACH: dllexit(); break;
  }
  return TRUE;
}