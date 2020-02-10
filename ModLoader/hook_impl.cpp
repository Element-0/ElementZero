#include "pch.h"
#include <hook.h>
#include "hook_init.h"

funchook *funchook_instance = funchook_create();

int HookFunction(void *oldfunc, void **poutold, void *newfunc) {
  auto code = funchook_prepare(funchook_instance, &oldfunc, newfunc);
  if (poutold) *poutold = oldfunc;
  return code;
}

void *GetServerSymbol(char const *name) {
  static auto handle = GetModuleHandle(nullptr);
  return GetProcAddress(handle, name);
}

FailedToHook::FailedToHook(int code) : exception(funchook_error_message(funchook_instance)) {}