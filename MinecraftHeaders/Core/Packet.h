#pragma once

#include <cstdint>
#include <string>

enum class PacketReliability { Relible, RelibleOrdered };
enum class StreamReadResult {};
enum class ExtendedStreamReadResult {};
enum class MinecraftPacketIds {};
class BinaryStream;
class ReadOnlyBinaryStream;

class Packet {
public:
  unsigned unk2                     = 2;                                 // 8
  PacketReliability reliableOrdered = PacketReliability::RelibleOrdered; // 12
  unsigned char clientSubId         = 0;                                 // 16
  uint64_t unk24                    = 0;                                 // 24
  unsigned incompressible           = 0;                                 // 32

  inline Packet(unsigned compress) : incompressible(!compress) {}
  inline Packet() {}
  inline virtual ~Packet() {}
  virtual MinecraftPacketIds getId() const              = 0;
  virtual std::string getName() const                   = 0;
  virtual void write(BinaryStream &) const              = 0;
  virtual StreamReadResult read(ReadOnlyBinaryStream &) = 0;
  inline virtual ExtendedStreamReadResult readExtended(ReadOnlyBinaryStream &stream) {
    return (ExtendedStreamReadResult) read(stream);
  }
  inline virtual bool disallowBatching() const { return false; }
};
