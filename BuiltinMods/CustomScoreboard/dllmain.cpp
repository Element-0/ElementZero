#include <cstdint>

#include <Packet/RemoveObjectivePacket.h>
#include <Packet/SetDisplayObjectivePacket.h>
#include <Packet/SetScorePacket.h>
#include <Scoreboard/ScoreboardId.h>
#include <Scoreboard/IdentityDefinition.h>

#include <dllentry.h>
#include <base/playerdb.h>
#include <mods/ScriptSupport/scriptapi.h>

void dllenter() {}
void dllexit() {}

using namespace Mod::Scripting;

enum class CustomScoreboardType { SIDEBAR, LIST };

template <CustomScoreboardType type> char const *GetObjectiveSlot();
template <> char const *GetObjectiveSlot<CustomScoreboardType::SIDEBAR>() { return "sidebar"; }
template <> char const *GetObjectiveSlot<CustomScoreboardType::LIST>() { return "list"; }
template <CustomScoreboardType type> char const *GetObjectiveName();
template <> char const *GetObjectiveName<CustomScoreboardType::SIDEBAR>() { return "ez:scoreboard:sidebar"; }
template <> char const *GetObjectiveName<CustomScoreboardType::LIST>() { return "ez:scoreboard:list"; }

template <CustomScoreboardType type>
void CreateCustomScorboard(Mod::PlayerEntry const &player, std::string const &name, bool order) {
  SetDisplayObjectivePacket pkt;
  pkt.slot           = GetObjectiveSlot<type>();
  pkt.objective_name = GetObjectiveName<type>();
  pkt.display_name   = name;
  pkt.criteria_name  = "dummy";
  pkt.order          = order ? ObjectiveSortOrder::asc : ObjectiveSortOrder::desc;
  player.player->sendNetworkPacket(pkt);
}

template <CustomScoreboardType type>
void DestroyCustomScorboard(Mod::PlayerEntry const &player) {
  RemoveObjectivePacket pkt;
  pkt.objective_name = GetObjectiveName<type>();
  player.player->sendNetworkPacket(pkt);
}

template <CustomScoreboardType type>
void SetCustomScorboard(
    Mod::PlayerEntry const &player, double /*uint64_t*/ id, std::string const &name, double /* uint32_t */ score) {
  IdentityDefinition def{{(uint64_t) id}, name};
  ScorePacketInfo info{def.id};
  info.obj_name      = GetObjectiveName<type>();
  info.fake_name     = name;
  info.type          = IdentityDefinition::Type::Fake;
  info.score         = score;
  SetScorePacket pkt = SetScorePacket::change({info});
  player.player->sendNetworkPacket(pkt);
}

template <CustomScoreboardType type>
void RemoveCustomScorboard(Mod::PlayerEntry const &player, double /*uint64_t*/ id) {
  SetScorePacket pkt = SetScorePacket::remove({(uint64_t) id});
  player.player->sendNetworkPacket(pkt);
}

template <CustomScoreboardType type> JsValueRef RegisterCustomScorboard() {
  JsObjectWrapper wrap;
  wrap["init"]   = &CreateCustomScorboard<type>;
  wrap["deinit"] = &DestroyCustomScorboard<type>;
  wrap["set"]    = &SetCustomScorboard<type>;
  wrap["remove"] = &RemoveCustomScorboard<type>;
  return *wrap;
}

static ModuleRegister reg("ez:scoreboard", [](JsObjectWrapper native) -> std::string {
  native["sidebar"] = RegisterCustomScorboard<CustomScoreboardType::SIDEBAR>();
  native["list"]    = RegisterCustomScorboard<CustomScoreboardType::LIST>();
  return R"js(
    export const sidebar = import.meta.native.sidebar;
    export const list = import.meta.native.list;
  )js";
});
