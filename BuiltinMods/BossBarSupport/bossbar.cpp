#include <cstdint>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>

#include <Packet/AddActorPacket.h>
#include <Packet/RemoveActorPacket.h>
#include <Packet/BossEventPacket.h>
#include <Packet/MoveActorAbsolutePacket.h>
#include <Packet/UpdateAttributesPacket.h>
#include <Packet/SetActorDataPacket.h>
#include <Actor/ActorUniqueID.h>
#include <Core/DataItem.h>
#include <Core/PacketSender.h>

#include <base/base.h>
#include <base/playerdb.h>
#include <base/scheduler.h>
#include <base/log.h>
#include <mods/BossBarSupport.h>

#include "global.h"

DEF_LOGGER("BossBar");

using namespace Mod::Scripting;

namespace Mod::Bossbar {

class Instance : public std::enable_shared_from_this<Instance> {
  uint64_t unqid;
  Config cfg;
  bool state = true;

  void createBossActor();
  void removeBossActor();
  void sendAttributes();
  void sendShowBossBar();
  void sendHideBossBar();
  void sendSetTexture();
  void sendSetName();
  void sendSetPercent();
  void updateBossActorPosition();
  void updateBossActorName();

public:
  Instance(Config const &cfg);
  ~Instance();
  void UpdateText(std::string const &text);
  void UpdatePercent(float percent);
  void UpdateColor(uint32_t color);
  void Show();
  void Hide();
  void Destory();
  void Refresh();
  void Rebuild(bool stage);
};

struct Set : Mod::AuxHolder {
  std::set<std::shared_ptr<Instance>> sets;
};

JsValueRef Handle::InitProto() {
  static LeakedHolder temp = IIFE([] {
    JsObjectWrapper proto;
    proto["valid"]         = &Handle::Valid;
    proto["show"]          = &Handle::Show;
    proto["hide"]          = &Handle::Hide;
    proto["destory"]       = &Handle::Destory;
    proto["updateText"]    = &Handle::UpdateText;
    proto["updatePercent"] = &Handle::UpdatePercent;
    return *proto;
  });
  return *temp;
}

Handle Handle::Create(const Config &cfg) {
  if (!cfg.entry.player) throw std::invalid_argument{"target player is null"};
  auto &pdb = Mod::PlayerDatabase::GetInstance();
  Handle ret;
  auto inst   = std::make_shared<Instance>(cfg);
  ret.ref     = inst;
  auto &entry = cfg.entry;
  pdb.GetAuxAuto<Set>(entry.player).sets.emplace(std::move(inst));
  return ret;
}

JsObjectWrapper Handle::CreateJsObject(Handle const &orig) {
  Handle *obj = new Handle;
  obj->ref    = orig.ref;
  return JsObjectWrapper::FromExternalObject(obj, InitProto());
}

void Handle::Show() {
  if (auto ptr = ref.lock(); ptr) ptr->Show();
}

void Handle::Hide() {
  if (auto ptr = ref.lock(); ptr) ptr->Hide();
}

void Handle::Destory() {
  if (auto ptr = ref.lock(); ptr) ptr->Destory();
}

void Handle::UpdateText(const std::string &text) {
  if (auto ptr = ref.lock(); ptr) ptr->UpdateText(text);
}

void Handle::UpdatePercent(float percent) {
  if (auto ptr = ref.lock(); ptr) ptr->UpdatePercent(percent);
}

void Handle::UpdateColor(uint32_t color) {
  if (auto ptr = ref.lock(); ptr) ptr->UpdateColor(color);
}

Instance::Instance(Config const &cfg) : cfg(cfg) {
  static std::random_device rd;
  static std::mt19937 eng{rd()};
  static std::uniform_int_distribution<uint64_t> distr{0xFF00000000, 0xFF7FFFFFFF};
  unqid = distr(eng);
  createBossActor();
  sendAttributes();
  sendShowBossBar();
  sendSetPercent();
  sendSetTexture();
}
Instance::~Instance() {
  sendHideBossBar();
  removeBossActor();
}
void Instance::Refresh() {
  updateBossActorPosition();
  sendShowBossBar();
  sendSetPercent();
  sendSetTexture();
}
void Instance::Rebuild(bool stage) {
  if (stage) {
    using namespace Mod::Scheduler;
    SetTimeOut(20_tick, [this](auto) {
      createBossActor();
      sendAttributes();
      sendShowBossBar();
      sendSetPercent();
      sendSetTexture();
    });
  } else {
    sendHideBossBar();
    removeBossActor();
  }
}

void Instance::Show() {
  state = true;
  sendShowBossBar();
}

void Instance::Hide() {
  state = false;
  sendHideBossBar();
}

void Instance::Destory() {
  sendHideBossBar();
  removeBossActor();
  auto &pdb = Mod::PlayerDatabase::GetInstance();
  pdb.GetAuxAuto<Set>(cfg.entry.player).sets.erase(shared_from_this());
}

void Instance::UpdateText(const std::string &text) {
  cfg.text = text;
  sendSetName();
}
void Instance::UpdatePercent(float percent) {
  cfg.percent = percent;
  sendSetPercent();
}
void Instance::UpdateColor(uint32_t color) {
  cfg.color = color;
  sendSetTexture();
}

void Instance::createBossActor() {
  AddActorPacket pkt;
  SynchedActorData syncedata;
  pkt.uid    = unqid;
  pkt.rid    = unqid;
  pkt.def_id = ActorDefinitionIdentifier{"minecraft:agent"};
  syncedata.append<int64_t>(ActorDataIDs::FLAGS, 0);
  syncedata.append<short>(ActorDataIDs::AIR, 400);
  syncedata.append<short>(ActorDataIDs::MAX_AIR, 400);
  syncedata.append<int64_t>(ActorDataIDs::LEAD_HOLDER, -1);
  syncedata.append<std::string>(ActorDataIDs::NAMETAG, cfg.text);
  syncedata.append<float>(ActorDataIDs::SCALE, 0);
  pkt.syncedata = &syncedata;
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::removeBossActor() {
  RemoveActorPacket pkt;
  pkt.uid = unqid;
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::sendAttributes() {
  UpdateAttributesPacket pkt;
  pkt.data.emplace_back(
      UpdateAttributesPacket::AttributeData{.value = 100.0f * cfg.percent, .max = 100.0f, .name = "minecraft:health"});
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::sendShowBossBar() {
  BossEventPacket pkt;
  pkt.owner   = unqid;
  pkt.type    = BossEventUpdateType::SHOW;
  pkt.name    = cfg.text;
  pkt.percent = cfg.percent;
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::sendHideBossBar() {
  BossEventPacket pkt;
  pkt.owner = unqid;
  pkt.type  = BossEventUpdateType::HIDE;
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::sendSetTexture() {
  BossEventPacket pkt;
  pkt.owner   = unqid;
  pkt.type    = BossEventUpdateType::TOGGLE_FOG;
  pkt.fog     = true;
  pkt.color   = cfg.color;
  pkt.overlay = cfg.color;
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::sendSetName() {
  BossEventPacket pkt;
  pkt.owner   = unqid;
  pkt.type    = BossEventUpdateType::TITLE;
  pkt.name    = cfg.text;
  pkt.percent = cfg.percent;
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::sendSetPercent() {
  BossEventPacket pkt;
  pkt.owner   = unqid;
  pkt.type    = BossEventUpdateType::HEALTH_PERCENT;
  pkt.percent = cfg.percent;
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::updateBossActorPosition() {
  MoveActorAbsolutePacket pkt;
  pkt.pos = cfg.entry.player->getPos();
  cfg.entry.player->sendNetworkPacket(pkt);
}

void Instance::updateBossActorName() {
  SetActorDataPacket pkt;
  pkt.rid = unqid;
  pkt.items.emplace_back(std::make_unique<DataItem2<std::string>>(ActorDataIDs::NAMETAG, cfg.text));
  cfg.entry.player->sendNetworkPacket(pkt);
}

} // namespace Mod::Bossbar

void OnChangeDimension(class Player *player, bool stage) {
  auto &pdb = Mod::PlayerDatabase::GetInstance();
  if (auto it = pdb.GetAux<Mod::Bossbar::Set>(player); it) {
    for (auto item : it->sets) item->Rebuild(stage);
  }
}