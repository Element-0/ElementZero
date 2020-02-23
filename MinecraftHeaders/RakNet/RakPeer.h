#pragma once

#include "RakNetTypes.h"

#include <hook.h>

namespace RakNet {

class RakPeer {
public:
  __declspec(dllimport) RakPeer();

  inline SystemAddress GetSystemAddressFromGuid(RakNetGUID guid) const {
    return CallServerClassMethod<SystemAddress>(
        "?GetSystemAddressFromGuid@RakPeer@RakNet@@UEBA?AUSystemAddress@2@URakNetGUID@2@@Z", this, guid);
  }
};

} // namespace RakNet