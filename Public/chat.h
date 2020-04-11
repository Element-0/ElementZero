#pragma once

#include "event.h"
#include "playerdb.h"

#ifdef ChatAPI_EXPORTS
#  define CHATAPI __declspec(dllexport)
#else
#  define CHATAPI __declspec(dllimport)
#endif

class Player;

namespace Mod {

class Chat : public RecursiveEventEmitter<
                 "chat"_sig, PlayerEntry const &, std::string &, std::string &, CallbackToken<std::string> &> {
  CHATAPI Chat();

public:
  CHATAPI static Chat &GetInstance();

  CHATAPI void SendBroadcast(std::string const &name, std::string const &);

  CHATAPI void SendAnnounce(std::string const &);
  CHATAPI void SendAnnounce(std::string const &, std::initializer_list<std::string> args);

  USING_RECEVENTEMITTER("chat", PlayerEntry const &, std::string &, std::string &, CallbackToken<std::string> &);
};

} // namespace Mod