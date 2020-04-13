#pragma once

#include <string>
#include <functional>

#include "../dll.h"

class NetworkPeer {
public:
  enum class Reliability : int {};
  enum class DataStatus : int { OK, BUSY };
  struct NetworkStatus;

  virtual ~NetworkPeer();
  virtual void sendPacket(std::string, NetworkPeer::Reliability, int) = 0;
  virtual DataStatus receivePacket(std::string &)                     = 0;
  virtual NetworkStatus getNetworkStatus()                            = 0;
  MCAPI virtual void update();
  MCAPI virtual void flush(std::function<void(void)> &&);
};