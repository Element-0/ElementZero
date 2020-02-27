#pragma once

class ActorFallPacket : public Packet {
public:
  ActorRuntimeID actorId;
  float fallDistance;
  bool inVoid;

  inline ~ActorFallPacket() {}
  __declspec(dllimport) virtual MinecraftPacketIds getId() const;
  __declspec(dllimport) virtual std::string getName() const;
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual PacketReadResult read(ReadOnlyBinaryStream &);
};