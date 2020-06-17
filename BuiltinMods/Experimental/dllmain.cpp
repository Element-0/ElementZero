#include <dllentry.h>
#include <hook.h>

#include <base/base.h>

#include "global.h"

DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

void PreInit() {
  if (settings.force_experimental_gameplay) {
    *GetServerSymbol<bool>("?mAllowExperimental@Enchant@@1_NA") = true;
    *GetServerSymbol<bool>("?mAllowExperimental@Item@@2_NA")    = true;
  }
  if (settings.education_feature) {
    GetServerSymbolWithOffset<PatchSpan<2>>(
        "?setupStandardServer@ServerCommands@@YAXAEAVMinecraft@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@"
        "D@2@@std@@1PEAVPermissionsFile@@@Z",
        0x26E)
        ->VerifyPatchFunction({0x74, 0x6D}, {0x90, 0x90});
  }
}