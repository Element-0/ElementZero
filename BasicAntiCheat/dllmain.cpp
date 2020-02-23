#include <dllentry.h>
#include <playerdb.h>
#include <hook.h>
#include <log.h>
#include <Net/NetworkIdentifier.h>

DEF_LOGGER("BAC");

void dllenter() {}
void dllexit() {}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
    NetworkIdentifier *netid, void *packet) {
  auto &db = Mod::PlayerDatabase::GetInstance().GetData().get<NetworkIdentifier>();
  auto it  = db.find(*netid);
  if (it != db.end()) {
    auto level = it->player->getCommandPermissionLevel();
    if (level > CommandPermissionLevel::Normal) {
      original(this, netid, packet);
    } else {
      LOGI("%s try to edit command block") % it->name;
    }
  }
}