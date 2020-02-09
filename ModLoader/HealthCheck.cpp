#include "pch.h"

#include "HealthCheck.h"
#include "hook_init.h"

void PrintErrorMessage() {
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0) return;
  LPWSTR messageBuffer = nullptr;
  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &messageBuffer, 0, NULL);
  std::wcerr << messageBuffer << std::endl;
  LocalFree(messageBuffer);
}

void HealthCheck() {
  auto lib = GetModuleHandle(nullptr);
  if (!lib) {
    std::wcerr << L"!!! ATTENTION !!!" << std::endl;
    std::wcerr << L"Failed to load library" << std::endl;
    PrintErrorMessage();
    exit(1);
    return;
  }
  auto fn = GetProcAddress(lib, "?isDedicatedServer@DedicatedServer@@EEBA_NXZ");
  if (!fn) {
    std::wcerr << L"!!! ATTENTION !!!" << std::endl;
    std::wcerr << L"Failed to load symbols from main executable!" << std::endl;
    PrintErrorMessage();
    exit(1);
    return;
  }
  std::wcerr << L"HealthCheck " << lib << L" " << fn << std::endl;

  auto base = LoadLibrary(L"Base.dll");
  if (!base) {
    std::wcerr << L"Failed to load base mod." << std::endl;
    PrintErrorMessage();
    exit(1);
    return;
  }
  funchook_install(funchook_instance, 0);
};