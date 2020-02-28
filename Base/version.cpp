#include <hook.h>
#include <string>

#ifndef EZVERSION
#  define EZVERSION ""
#endif

THook(
    std::string,
    "?getServerVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ") {
  return original() + (" with element-0 (" EZVERSION ")");
}