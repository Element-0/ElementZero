// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "HealthCheck.h"
#include <iostream>
#include <combaseapi.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    std::wcout << L"Injected!" << std::endl;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    HealthCheck();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH: break;
  }
  return TRUE;
}