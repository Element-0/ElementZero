#pragma once

#include "../Core/mce.h"
#include <string>

namespace RakNet {

using UniqueIDType = unsigned char;
using SystemIndex  = unsigned short;
using RPCIndex     = unsigned char;

struct SystemAddress {
public:
  __declspec(dllimport) SystemAddress();

  unsigned short type;
  char filler[0x7e];
  unsigned unk128;

  __declspec(dllimport) SystemAddress &operator=(SystemAddress const &);
  __declspec(dllimport) bool operator==(SystemAddress const &) const;

private:
  __declspec(dllimport) void ToString_New(bool writePort, char *dest, char portDelineator) const;

public:
  inline std::string ToString() {
    char buffer[256];
    ToString_New(true, buffer, '|');
    return {buffer};
  }
};

struct __declspec(dllimport) RakNetGUID {
  RakNetGUID();
  uint64_t g;
  SystemIndex systemIndex;
};

static_assert(sizeof(RakNetGUID) == 16);

struct Packet {
  SystemAddress address;
  RakNetGUID guid;
  unsigned int length;
  uint32_t bitSize;
  unsigned char *data;
};

} // namespace RakNet