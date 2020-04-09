#pragma once
#include <boost/functional/hash.hpp>

#include <hook.h>

#include "../Core/mce.h"
#include "../RakNet/RakNetTypes.h"
#include "../dll.h"

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class NetworkIdentifier {
public:
  enum class Type : char {
    GUID  = 0,
    IPv4  = 1,
    IPv6  = 2,
    Empty = 3,
  };
  RakNet::RakNetGUID guid;
  uint16_t port;
  char filler[0x76];
  uint64_t unk136;
  Type type; // 144

  MCAPI bool operator==(NetworkIdentifier const &) const;
  MCAPI uint64_t getHash() const;
  MCAPI std::string getAddress() const;

  BASEAPI RakNet::SystemAddress getRealAddress() const;
  BASEAPI void kick(std::string const &reason) const;
};

static_assert(offsetof(NetworkIdentifier, unk136) == 136);
static_assert(offsetof(NetworkIdentifier, type) == 144);

namespace std {

template <> struct hash<NetworkIdentifier> {
  size_t operator()(const NetworkIdentifier &val) const { return val.getHash(); }
};

} // namespace std

namespace boost {

template <> struct hash<NetworkIdentifier> {
  size_t operator()(const NetworkIdentifier &val) const { return val.getHash(); }
};

} // namespace boost