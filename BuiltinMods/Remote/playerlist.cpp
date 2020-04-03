#include <flatbuffers/flatbuffers.h>
#include <stdexcept>
#include <ws-gw.h>

#include <playerdb.h>
#include <remote.h>

#include "global.h"

#include "proto/player_generated.h"

flatbuffers::Offset<Mod::proto::PlayerEntity>
BuildPlayerEntity(flatbuffers::FlatBufferBuilder &builder, Mod::PlayerEntry const &entry) {
  Mod::proto::PlayerEntityT ent;
  ent.xuid    = entry.xuid;
  ent.uuid    = entry.uuid;
  ent.name    = entry.name;
  ent.address = entry.netid.getRealAddress().ToString();
  return Mod::proto::PlayerEntity::Pack(builder, &ent);
}

template <Mod::proto::PlayerEvent EVENT> static void OnPlayerChange(Mod::PlayerEntry const &entry) {
  static auto &instance = Mod::Remote::GetInstance();
  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(CreatePlayerEventPacket(builder, EVENT, BuildPlayerEntity(builder, entry).Union()));
  instance.Broadcast("player_change", builder);
}

static WsGw::Buffer GetPlayerList(WsGw::BufferView const &) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  flatbuffers::FlatBufferBuilder builder;
  using namespace Mod::proto;
  std::vector<flatbuffers::Offset<Mod::proto::PlayerEntity>> vec;
  for (auto ent : db.GetData()) vec.emplace_back(BuildPlayerEntity(builder, ent));
  builder.Finish(CreatePlayerListDirect(builder, &vec));
  return builder;
}

static WsGw::Buffer FindPlayer(WsGw::BufferView const &view) {
  using namespace Mod::proto;
  auto &db = Mod::PlayerDatabase::GetInstance();
  flatbuffers::Verifier verifier{view.data(), view.size()};
  auto find = flatbuffers::GetRoot<FindPlayerPacket>(view.data());
  if (!find->Verify(verifier)) throw std::runtime_error{"Failed to parse payload"};
  FindPlayerPacketT pkt;
  find->UnPackTo(&pkt);
  std::optional<struct Mod::PlayerEntry> found;
  if (auto it = pkt.find.AsFindXUID(); it) {
    found = db.Find(it->xuid);
  } else if (auto it = pkt.find.AsFindUUID(); it) {
    found = db.Find(it->uuid);
  } else if (auto it = pkt.find.AsFindNAME(); it) {
    found = db.Find(it->name);
  } else {
    throw std::runtime_error{"Unssupported find type"};
  }
  flatbuffers::FlatBufferBuilder builder;
  if (found) {
    builder.Finish(CreateFindResult(builder, BuildPlayerEntity(builder, *found)));
  } else {
    builder.Finish(CreateFindResult(builder));
  }
  return builder;
}

void InitPlayerlistHook() {
  auto &db       = Mod::PlayerDatabase::GetInstance();
  auto &instance = Mod::Remote::GetInstance();
  db.AddListener(SIG("joined"), &OnPlayerChange<Mod::proto::PlayerEvent::joined>);
  db.AddListener(SIG("left"), &OnPlayerChange<Mod::proto::PlayerEvent::left>);
  instance.AddMethod("playerlist", &GetPlayerList);
  instance.AddMethod("findplayer", &FindPlayer);
}