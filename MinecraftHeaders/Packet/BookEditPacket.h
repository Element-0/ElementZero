#pragma once

#include <string>

#include "../Core/Packet.h"
#include "../dll.h"

class BookEditPacket : public Packet {
public:
  enum struct Action { REPLACE_PAGE, ADD_PAGE, DELETE_PAGE, SWAP_PAGES, SIGN_BOOK, UNKNOWN = -1 };
  Action action = Action::UNKNOWN;
  int slot = 0, page = 0, secondary_page = 0;
  std::string a, b, sign;

  inline ~BookEditPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual StreamReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(BookEditPacket, action) == 40);
static_assert(offsetof(BookEditPacket, slot) == 44);
static_assert(offsetof(BookEditPacket, a) == 56);
static_assert(offsetof(BookEditPacket, b) == 88);
static_assert(offsetof(BookEditPacket, sign) == 120);