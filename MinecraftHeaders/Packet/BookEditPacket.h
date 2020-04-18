#pragma once

#include <string>

#include "../Core/Packet.h"
#include "../dll.h"

class BookEditPacket : public Packet {
public:
  enum struct Action  { REPLACE_PAGE, ADD_PAGE, DELETE_PAGE, SWAP_PAGES, SIGN_BOOK, UNKNOWN = -1 };
  Action action;
  int slot, page, secondary_page;
  std::string a, b, sign;

  inline ~BookEditPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(BookEditPacket, action) == 40);
static_assert(offsetof(BookEditPacket, slot) == 44);
static_assert(offsetof(BookEditPacket, a) == 56);
static_assert(offsetof(BookEditPacket, b) == 88);
static_assert(offsetof(BookEditPacket, sign) == 120);