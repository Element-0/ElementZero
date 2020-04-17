#pragma once

#include <string>

#include "../Core/Packet.h"
#include "../dll.h"

class BookEditPacket : public Packet {
  enum struct Action : char { REPLACE_PAGE, ADD_PAGE, DELETE_PAGE, SWAP_PAGES, SIGN_BOOK, UNKNOWN = -1 };
  Action action;
  int slot, page, secondary_page;
  std::string a, b, sign;

  inline ~BookEditPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};