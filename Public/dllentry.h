#pragma once

#include <windows.h>

#include <filesystem>

#include "yaml.h"

// On dll load
void dllenter();
// On dll unload (PS: You should never use any minecraft interface or mod api in this function)
void dllexit();

// Called if config is present
extern "C" __declspec(dllexport) void ApplySettings(YAML::Node const &node);
// Called if config is not present
extern "C" __declspec(dllexport) bool GenerateSettings(YAML::Node &node);
// Called immediately after one of the first two functions is called
extern "C" __declspec(dllexport) void PreInit();
// Called after all modules have been loaded
extern "C" __declspec(dllexport) void PostInit();
// Called after world path have been resolved
extern "C" __declspec(dllexport) void WorldInit(std::filesystem::path const &);
// Called after server started
extern "C" __declspec(dllexport) void ServerStart();
// Called before the server is stopped
extern "C" __declspec(dllexport) void BeforeUnload();
// Called after reload settings
extern "C" __declspec(dllexport) void AfterReload();

#define DEFAULT_SETTINGS(settings)                                                                                     \
  void ApplySettings(YAML::Node const &node) { ReadYAML(settings, node); }                                             \
  bool GenerateSettings(YAML::Node &node) { return WriteYAML(settings, node); }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: dllenter(); break;
  case DLL_THREAD_ATTACH: break;
  case DLL_THREAD_DETACH: break;
  case DLL_PROCESS_DETACH: dllexit(); break;
  }
  return TRUE;
}