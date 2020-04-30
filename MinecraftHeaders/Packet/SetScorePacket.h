#pragma once

#include <vector>

#include "../Core/Packet.h"
#include "../Scoreboard/ScorePacketInfo.h"
#include "../Scoreboard/Objective.h"
#include "../dll.h"

enum class ScorePacketType : char { set, remove };

class SetScorePacket : public Packet {
public:
  std::vector<ScorePacketInfo> infos;

  inline SetScorePacket() {}
  MCAPI SetScorePacket(ScorePacketType, ScoreboardId const &, Objective const &);
  inline SetScorePacket(ScoreboardId const &id) : infos{id} {}
  inline SetScorePacket(std::vector<ScorePacketInfo> infos) : infos(std::move(infos)) {}

  inline ~SetScorePacket() {}
  MCAPI virtual MinecraftPacketIds getId() const;
  MCAPI virtual std::string getName() const;
  MCAPI virtual void write(BinaryStream &) const;
  MCAPI virtual PacketReadResult read(ReadOnlyBinaryStream &);
};