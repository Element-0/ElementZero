#include <sstream>

#include <Packet/ModalFormRequestPacket.h>
#include <Packet/ModalFormResponsePacket.h>

#include <hook.h>

#include "include/mods/FormUI.h"

struct FormAuxData : Mod::AuxHolder {
  std::map<int, std::function<void(Json::Value const &)>> map;
};

void Mod::FormUI::SendModalForm(
    Mod::PlayerEntry const &target, Json::Value const &req, std::function<void(Json::Value const &)> resp) {
  Json::FastWriter writer;
  auto data = writer.write(req);
  ModalFormRequestPacket pkt;
  pkt.id      = 0;
  pkt.content = data;
  auto &aux   = Mod::PlayerDatabase::GetInstance().GetAuxAuto<FormAuxData>(target.player);
  aux.map.emplace(0, resp);
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
