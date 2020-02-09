// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

extern "C" __declspec(dllexport) HMODULE ServerModule() {
    static auto value = GetModuleHandle(L"bedrock_server_mod.exe");
    return value;
}

constexpr auto server_entry = "?start@DedicatedServer@@QEAA?AW4StartResult@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";

struct DedicatedServer {
    enum struct StartResult { OK, Error };

    __declspec(dllimport) StartResult start(std::string const&);
};

template <typename T> void* void_cast(T input) {
    union {
        T src;
        void* dst;
    } u;
    u.src = input;
    return u.dst;
}

static void entry() {
    // DEBUG purpose
    auto sym = GetProcAddress(ServerModule(), server_entry);
    std::wcout << L"Debug" << std::endl;
    std::wcout << L"<-" << sym << std::endl;
    std::wcout << L"->" << void_cast(&DedicatedServer::start) << std::endl;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        entry();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

