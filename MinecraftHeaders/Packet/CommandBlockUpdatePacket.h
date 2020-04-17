#pragma once

#include "../Core/Packet.h"
#include "../Actor/ActorRuntimeID.h"
#include "../Math/BlockPos.h"
#include "../dll.h"

class CommandBlockUpdatePacket : public Packet {
public:
  alignas(8) BlockPos pos;
  unsigned short mode;
  bool redstone;
  bool conditional;
  ActorRuntimeID runtime_id;
  std::string command, output, raw_name;
  int delay;
  bool track_output;
  bool execute_first_tick;
  bool is_block;

  inline ~CommandBlockUpdatePacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};

static_assert(offsetof(CommandBlockUpdatePacket, mode) == 52);
static_assert(offsetof(CommandBlockUpdatePacket, redstone) == 54);
static_assert(offsetof(CommandBlockUpdatePacket, conditional) == 55);
static_assert(offsetof(CommandBlockUpdatePacket, command) == 64);
static_assert(offsetof(CommandBlockUpdatePacket, delay) == 160);
static_assert(offsetof(CommandBlockUpdatePacket, track_output) == 164);
static_assert(offsetof(CommandBlockUpdatePacket, is_block) == 166);