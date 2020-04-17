#pragma once

#include "../Core/Packet.h"
#include "../Command/CommandOriginData.h"
#include "../dll.h"

class CommandRequestPacket : public Packet {
public:
  std::string command;
  CommandOriginData data;
  bool flag;

  inline ~CommandRequestPacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(CommandRequestPacket, command) == 40);
static_assert(offsetof(CommandRequestPacket, data) == 72);
static_assert(offsetof(CommandRequestPacket, flag) == 136);