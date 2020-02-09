#include "pch.h"
#include "HealthCheck.h"

void HealthCheck() {
    auto lib = GetModuleHandle(L"bedrock_server_mod.exe");
    if (!lib) {
        std::wcerr << L"!!! ATTENTION !!!" << std::endl;
        std::wcerr << L"Failed to load library" << std::endl;
        exit(1);
        return;
    }
    auto fn = GetProcAddress(lib, "?isDedicatedServer@DedicatedServer@@EEBA_NXZ");
    if (!fn) {
        std::wcerr << L"!!! ATTENTION !!!" << std::endl;
        std::wcerr << L"Failed to load symbols from main executable!" << std::endl;
        exit(1);
        return;
    }
    std::wcerr << L"HealthCheck " << lib << L" " << fn << std::endl;

    auto base = LoadLibrary(L"Base.dll");
    if (!base) {
        std::wcerr << L"Failed to load base mod" << std::endl;
    }
};