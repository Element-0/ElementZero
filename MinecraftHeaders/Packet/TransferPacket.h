#pragma once

#include "../Core/Packet.h"
#include <hook.h>

class TransferPacket : public Packet {
  std::string address;
  int port;

public:
  TransferPacket(std::string address, int port) : address(address), port(port) {}
  inline virtual ~TransferPacket() {}
  __declspec(dllimport) virtual MinecraftPacketIds getId() const;
  __declspec(dllimport) virtual std::string getName() const;
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual PacketReadResult read(ReadOnlyBinaryStream &);
  virtual bool disallowBatching() const { return false; }
};