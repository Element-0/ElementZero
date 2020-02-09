#include "pch.h"

class DedicatedServer {
public:
  __declspec(dllexport) void asyncStop() { *(((bool *) this) + 48) = true; }
};

static DedicatedServer *mDedicatedServer = nullptr;

extern "C" __declspec(dllexport) DedicatedServer *GetDedicatedServer() { return mDedicatedServer; }

TInstanceHook(
  int,
  ?start@DedicatedServer@@QEAA?AW4StartResult@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z,
  DedicatedServer,
  void *name) {
  mDedicatedServer = this;
  std::cout << "hooked: " << (void *) name << std::endl;
  return original(this, name);
}

static BOOL ConsoleCtrlHandler(DWORD type) {
  static bool stopping = false;
  if (type == CTRL_C_EVENT) {
    if (mDedicatedServer) {
      if (stopping) return TRUE;
      mDedicatedServer->asyncStop();
      std::wcout << L"Requested to stop" << std::endl;
      stopping = true;
      return TRUE;
    }
    ExitProcess(0);
  }
  return TRUE;
}

static void entry() {
  std::thread::id this_id = std::this_thread::get_id();
  std::wcout << L"Current thread id: " << this_id << std::endl;
  std::wcout << L"Base mod loaded, setting up CtrlC handler..." << std::endl;
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: entry(); break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH: break;
  }
  return TRUE;
}
