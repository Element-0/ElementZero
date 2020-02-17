#include "pch.h"
#include <hook.h>
#include "hook_init.h"
#include <map>

funchook *funchook_instance = funchook_create();

static std::map<void *, void **> hookmap;

int HookFunction(void *oldfunc, void **poutold, void *newfunc) {
  auto source = oldfunc;
  auto it = hookmap.find(oldfunc);
  if (it != hookmap.end()) {
    *poutold = *it->second;
    *it->second = newfunc;
    return 0;
  }
  auto code = funchook_prepare(funchook_instance, &oldfunc, newfunc);
  if (code == 0) {
    *poutold = oldfunc;
    hookmap.emplace(source, poutold);
  }
  return code;
}

void *GetServerSymbol(char const *name) {
  static auto handle = GetModuleHandle(nullptr);
  return (void *) GetProcAddress(handle, name);
}

FailedToHook::FailedToHook(int code) : exception(funchook_error_message(funchook_instance)) {}