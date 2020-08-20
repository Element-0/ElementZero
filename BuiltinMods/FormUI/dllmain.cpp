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

class FormAuxData : public Mod::AuxHolder {
public:
  using callback_type = std::function<void(Json::Value const &)>;

private:
  std::map<int, callback_type> data;

public:
  int alloc() {
    auto ret = rand();
    if (data.find(ret) != data.end()) return alloc();
    return ret;
  }
  template <typename T> void insert(int k, T &&fn) { data.emplace(k, std::forward<T &&>(fn)); }
  std::optional<callback_type> find(int k) {
    if (auto it = data.find(k); it != data.end()) return {it->second};
    return std::nullopt;
  }
  void erase(int k) { data.erase(k); }
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
  aux.insert(id, std::move(resp));
  target.player->sendNetworkPacket(pkt);
}

TClasslessInstanceHook(
    void,
    "?handle@?$PacketHandlerDispatcherInstance@VModalFormResponsePacket@@$0A@@@UEBAXAEBVNetworkIdentifier@@"
    "AEAVNetEventCallback@@AEAV?$shared_ptr@VPacket@@@std@@@Z",
    NetworkIdentifier const &netid, void *cb, std::shared_ptr<ModalFormResponsePacket> &pkt) {
  auto &db = Mod::PlayerDatabase::GetInstance();
  if (auto entry = db.Find(netid)) {
    auto &data    = db.GetAuxAuto<FormAuxData>(entry->player);
    auto callback = data.find(pkt->id);
    if (!callback) return;
    Json::Reader reader;
    Json::Value root;
    std::istringstream iss{pkt->content};
    reader.parse(iss, root);
    try {
      (*callback)(root);
      if (root.isNull() || root.isIntegral()) { data.erase(pkt->id); }
    } catch (...) { data.erase(pkt->id); }
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
        auto id   = 0;
        ServerSettingsResponsePacket pkt;
        pkt.id      = id;
        pkt.content = data;
        aux.insert(id, cb);
        entry->player->sendNetworkPacket(pkt);
      }
    }
  }
}