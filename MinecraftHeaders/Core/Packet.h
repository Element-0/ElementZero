#pragma once

#include <cstdint>
#include <string>

enum class PacketReliability { Relible, RelibleOrdered };
enum class PacketReadResult {};
enum class MinecraftPacketIds {};
class BinaryStream;
class ReadOnlyBinaryStream;

#pragma pack(push, 1)
class Packet {
public:
  unsigned unk2                     = 2;                                 // 8
  PacketReliability reliableOrdered = PacketReliability::RelibleOrdered; // 12
  unsigned char clientSubId         = 0;                                 // 16
  char _pad17[7];
  uint64_t unk24                    = 0;                                 // 24
  unsigned compressible             = 0;                                 // 32

  inline Packet(unsigned compress) : compressible(compress) {}
  inline Packet() {}
  inline virtual ~Packet() {}
  virtual MinecraftPacketIds getId() const              = 0;
  virtual std::string getName() const                   = 0;
  virtual void write(BinaryStream &) const              = 0;
  virtual PacketReadResult read(ReadOnlyBinaryStream &) = 0;
  virtual bool disallowBatching() const { return false; }
};
#pragma pack(pop)

static_assert(sizeof(Packet) == 36);