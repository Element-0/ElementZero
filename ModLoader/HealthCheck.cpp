#include "pch.h"

#include "HealthCheck.h"
#include "hook_init.h"
#include <filesystem>

#include <WinSock2.h>

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

static_assert(sizeof(std::string) == 32);

void HealthCheck() {
  namespace fs = std::filesystem;
  WCHAR exepath[MAX_PATH];
  GetModuleFileName(NULL, exepath, MAX_PATH);
  fs::path xpath = exepath;
  xpath          = xpath.parent_path();
  SetCurrentDirectory(xpath.c_str());
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
  std::wcerr << L"HealthCheck " << lib << L" " << (void *) fn << std::endl;

  auto base = LoadLibrary(L"Base.dll");
  if (!base) {
    std::wcerr << L"Failed to load base mod." << std::endl;
    PrintErrorMessage();
    exit(1);
    return;
  }
  funchook_install(funchook_instance, 0);
};