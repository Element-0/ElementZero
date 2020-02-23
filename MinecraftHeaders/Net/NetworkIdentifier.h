#pragma once
#include "../Core/mce.h"
#include "../RakNet/RakNetTypes.h"
#include <boost/functional/hash.hpp>

class __declspec(dllimport) NetworkIdentifier {
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

  bool operator==(NetworkIdentifier const &) const;
  uint64_t getHash() const;
  std::string getAddress() const;
};

static_assert(offsetof(NetworkIdentifier, unk136) == 136);
static_assert(offsetof(NetworkIdentifier, type) == 144);

namespace std {

template <> struct hash<NetworkIdentifier> {
  constexpr size_t operator()(const NetworkIdentifier &val) const { return val.getHash(); }
};

} // namespace std

namespace boost {

template <> struct hash<NetworkIdentifier> {
  constexpr size_t operator()(const NetworkIdentifier &val) const { return val.getHash(); }
};

} // namespace boost