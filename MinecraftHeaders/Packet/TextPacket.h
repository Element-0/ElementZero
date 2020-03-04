#pragma once

#include "../Core/Packet.h"
#include "../TextObject/TextObjectRoot.h"
#include "../TextObject/TextObjectLocalizedTextWithParams.h"
#include <hook.h>

#include <string>
#include <vector>

enum class TextPacketType : char {
  Raw           = 0,
  Chat          = 1,
  JukeboxPopup  = 4,
  SystemMessage = 6,
  Whisper       = 7,
  Announcement  = 8,
  TextObject    = 9,
};

class TextPacket : public Packet {
public:
  TextPacketType type;
  std::string source, content;
  std::vector<std::string> args;
  bool translated;
  std::string xuid;
  std::string unknown;
  template <TextPacketType type, bool translated = true>
  static inline TextPacket createTextPacket(std::string content) {
    TextPacket pkt;
    pkt.type       = type;
    pkt.translated = translated;
    pkt.content    = content;
    return pkt;
  }
  template <TextPacketType type, bool translated = true>
  static inline TextPacket createTextPacket(std::string source, std::string content) {
    TextPacket pkt;
    pkt.type       = type;
    pkt.translated = translated;
    pkt.source     = source;
    pkt.content    = content;
    return pkt;
  }
  template <TextPacketType type, bool translated = true>
  static inline TextPacket createTextPacket(std::string source, std::string content, std::string xuid) {
    TextPacket pkt;
    pkt.type       = type;
    pkt.translated = translated;
    pkt.source     = source;
    pkt.content    = content;
    pkt.xuid       = xuid;
    return pkt;
  }
  __declspec(dllimport) static TextPacket
      createTextObjectMessage(TextObjectRoot const &, std::string const &xuid = "", std::string const &unk2 = "");

  static inline TextPacket
  createTranslatedMessageWithParams(std::string const &text, std::initializer_list<std::string> args = {}) {
    TextObjectRoot root;
    root.addChild(TextObjectLocalizedTextWithParams::build(text, args));
    return createTextObjectMessage(root);
  }
  inline TextPacket() {}
  inline ~TextPacket() {}
  __declspec(dllimport) virtual MinecraftPacketIds getId() const;
  __declspec(dllimport) virtual std::string getName() const;
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual PacketReadResult read(ReadOnlyBinaryStream &);
};