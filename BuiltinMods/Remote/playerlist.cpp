#include <flatbuffers/flatbuffers.h>

#include <playerdb.h>

#include "global.h"

#include "proto/player_generated.h"

template<Mod::proto::PlayerEvent EVENT>
static void OnPlayerChange(Mod::PlayerEntry const &entry) {
  flatbuffers::FlatBufferBuilder builder;
  using namespace Mod::proto;
  PlayerEntityT ent;
  ent.xuid    = entry.xuid;
  ent.uuid    = entry.uuid;
  ent.name    = entry.name;
  ent.address = entry.netid.getRealAddress().ToString();
  builder.Finish(CreatePlayerEventPacket(builder, EVENT, PlayerEntity::Pack(builder, &ent).Union()));
  state->srv.Broadcast("player_change", builder);
}

void InitPlayerlistHook() {
  auto &db = Mod::PlayerDatabase::GetInstance();
  db.AddListener(SIG("joined"), &OnPlayerChange<Mod::proto::PlayerEvent::joined>);
  db.AddListener(SIG("left"), &OnPlayerChange<Mod::proto::PlayerEvent::left>);
}