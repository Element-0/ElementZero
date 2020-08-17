#include <sstream>
#include <cstdlib>

#include <Packet/ModalFormRequestPacket.h>
#include <Packet/ModalFormResponsePacket.h>
#include <Packet/ServerSettingsRequestPacket.h>
#include <Packet/ServerSettingsResponsePacket.h>

#include <hook.h>
#include <base/log.h>

#include "include/mods/FormUI.h"

static void (Mod::FormUI::*emit)(
    sigt<"server_settings"_sig>, Mod::PlayerEntry const &,
    Mod::CallbackToken<std::pair<Json::Value, std::function<void(Json::Value const &)>>> &);

static Mod::FormUI *inited = nullptr;

Mod::FormUI::FormUI() {
  inited = this;
  emit   = &FormUI::Emit;
}

Mod::FormUI &Mod::FormUI::GetInstance() {
  static FormUI instance;
  return instance;
}

struct FormAuxData : Mod::AuxHolder {
  std::map<int, std::function<void(Json::Value const &)>> map;

  int alloc() {
    auto ret = rand();
    if (map.find(ret) != map.end()) return alloc();
    return ret;
  }
};

void Mod::FormUI::SendModalForm(
    Mod::PlayerEntry const &target, Json::Value const &req, std::function<void(Json::Value const &)> resp) {
  Json::FastWriter writer;
  auto data = writer.write(req);
  auto &aux = Mod::PlayerDatabase::GetInstance().GetAuxAuto<FormAuxData>(target.player);
  auto id   = aux.alloc();
  ModalFormRequestPacket pkt;
  pkt.id      = id;
  pkt.content = data;
  aux.map.emplace(id, resp);
  target.player->sendNetworkPacket(pkt);
}

TClasslessInstanceHook(
    void,
    "?handle@?$PacketHandlerDispatcherInstance@VModalFormResponsePacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@"
    "AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
    NetworkIdentifier const &netid, void *cb, std::shared_ptr<ModalFormResponsePacket> &pkt) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  if (auto entry = db.Find(netid)) {
    auto &map = db.GetAuxAuto<FormAuxData>(entry->player).map;
    if (auto it = map.find(pkt->id); it != map.end()) {
      Json::Reader reader;
      Json::Value root;
      std::istringstream iss{pkt->content};
      reader.parse(iss, root);
      it->second(root);
      if (root.isNull()) { map.erase(it); }
    }
  }
}

TClasslessInstanceHook(
    void,
    "?handle@?$PacketHandlerDispatcherInstance@VServerSettingsRequestPacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@"
    "AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
    NetworkIdentifier const &netid, void *cb, std::shared_ptr<ServerSettingsRequestPacket> &pkt) {
  if (inited) {
    auto &db = Mod::PlayerDatabase::GetInstance();
    if (auto entry = db.Find(netid)) {
      Mod::CallbackToken<std::pair<Json::Value, std::function<void(Json::Value const &)>>> token;
      (inited->*emit)(SIG("server_settings"), *entry, token);
      if (token) {
        auto &[json, cb] = *token;
        Json::FastWriter writer;
        auto data = writer.write(json);
        auto &aux = Mod::PlayerDatabase::GetInstance().GetAuxAuto<FormAuxData>(entry->player);
        auto id   = aux.alloc();
        ServerSettingsResponsePacket pkt;
        pkt.id      = id;
        pkt.content = data;
        aux.map.emplace(id, cb);
        entry->player->sendNetworkPacket(pkt);
      }
    }
  }
}