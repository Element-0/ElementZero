#include <dllentry.h>
#include <hook.h>

#include "global.h"

void PreInit() {
  mod_enabled = true;
  GetServerSymbolWithOffset<PatchSpan<6>>(
      "?onServerThreadStarted@MinecraftServerScriptEngine@@UEAA?AW4EventResult@@AEAVServerInstance@@@Z", 0x3C)
      ->VerifyPatchFunction({0x0F, 0x84, 0x5F, 0x01, 0x00, 0x00}, NopFilled{});
  GetServerSymbolWithOffset<PatchSpan<6>>(
      "?onServerThreadStarted@MinecraftServerScriptEngine@@UEAA?AW4EventResult@@AEAVServerInstance@@@Z", 0x49)
      ->VerifyPatchFunction({0x0F, 0x84, 0x52, 0x01, 0x00, 0x00}, NopFilled{});
  GetServerSymbolWithOffset<PatchSpan<6>>(
      "?onServerThreadStarted@MinecraftServerScriptEngine@@UEAA?AW4EventResult@@AEAVServerInstance@@@Z", 0x59)
      ->VerifyPatchFunction({0x0F, 0x84, 0x42, 0x01, 0x00, 0x00}, NopFilled{});
}