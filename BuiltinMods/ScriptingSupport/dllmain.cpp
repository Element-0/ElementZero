#include <dllentry.h>
#include <hook.h>

#include "global.h"

void dllenter() {}
void dllexit() {}

void PreInit() {
  mod_enabled = true;
  GetServerSymbolWithOffset<PatchSpan<6>>(
      "?onServerThreadStarted@MinecraftServerScriptEngine@@UEAA?AW4EventResult@@AEAVServerInstance@@@Z", 0x39)
      ->VerifyPatchFunction({0x0F, 0x84, 0x59, 0x01, 0x00, 0x00}, NopFilled{});

  GetServerSymbolWithOffset<PatchSpan<6>>(
      "?onServerThreadStarted@MinecraftServerScriptEngine@@UEAA?AW4EventResult@@AEAVServerInstance@@@Z", 0x46)
      ->VerifyPatchFunction({0x0F, 0x84, 0x4C, 0x01, 0x00, 0x00}, NopFilled{});

  GetServerSymbolWithOffset<PatchSpan<6>>(
      "?onServerThreadStarted@MinecraftServerScriptEngine@@UEAA?AW4EventResult@@AEAVServerInstance@@@Z", 0x56)
      ->VerifyPatchFunction({0x0F, 0x84, 0x3C, 0x01, 0x00, 0x00}, NopFilled{});
}