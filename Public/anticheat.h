#pragma once

#include <Actor/Player.h>

#include "event.h"
#include "playerdb.h"

#ifdef BasicAntiCheat_EXPORTS
#  define BACAPI __declspec(dllexport)
#else
#  define BACAPI __declspec(dllimport)
#endif

namespace Mod {

class AntiCheat : RecursiveEventEmitter<"detected"_sig, std::string_view, PlayerEntry const &> {
  BACAPI AntiCheat();

public:
  BACAPI static AntiCheat &GetInstance();

  USING_RECEVENTEMITTER("detected", std::string_view, PlayerEntry const &);
};

} // namespace Mod