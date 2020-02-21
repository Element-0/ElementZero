#pragma once

#include "event.h"

#ifdef Essentials_EXPORTS
#  define ESSAPI __declspec(dllexport)
#else
#  define ESSAPI __declspec(dllimport)
#endif

namespace Mod {

class Chat : public EventEmitter<"chat"_sig, Player const &, std::string &, bool &> {
  ESSAPI Chat();

public:
  ESSAPI static Chat &GetInstance();

  USING_EVENTEMITTER("chat", Player const &, std::string &, bool &);
};

} // namespace Mod