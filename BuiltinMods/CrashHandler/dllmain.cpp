#include <dllentry.h>
#include <hook.h>
#include <log.h>

DEF_LOGGER("CrashHandler");

void dllenter() {}
void dllexit() {}

THook(
    void, "?initialize@CrashHandler@@SAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@00@Z",
    std::string const &a, std::string const &b, std::string const &c) {
  LOGV("Replace crash report endpoint");
  original(a, "https://rink.hockeyapp.net/api/2/apps/64afd4a74b1e437c92eb88e43b131769/crashes/upload", c);
}