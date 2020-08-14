#include <stdexcept>

#include <boost/format.hpp>

#include <Core/ServerInstance.h>

#include <base/playerdb.h>
#include <base/base.h>

#include "global.h"

static RegisterAPI reg("ChatAPI", false, [] {
  auto &playerdb = Mod::PlayerDatabase::GetInstance();
  playerdb.AddListener(SIG("joined"), [](Mod::PlayerEntry const &player) {
    auto res = boost::format("%s\n%s\n%s") % player.name % player.xuid % player.uuid;
    client->notify("player-join", res.str());
  });
  playerdb.AddListener(SIG("left"), [](Mod::PlayerEntry const &player) {
    auto res = boost::format("%s\n%s\n%s") % player.name % player.xuid % player.uuid;
    client->notify("player-left", res.str());
  });
  client->register_handler("find-player", [](std::string_view content) -> std::string {
    auto &playerdb = Mod::PlayerDatabase::GetInstance();
    if (auto opt = playerdb.GeneralFind(std::string{content}); opt) {
      auto res = boost::format("online\n%s\n%s\n%s") % opt->name % opt->xuid % opt->uuid;
      return res.str();
    }
    if (auto opt = playerdb.GeneralFindOffline(std::string{content}); opt) {
      auto res = boost::format("offline\n%s\n%s\n%s") % opt->name % opt->xuid % opt->uuid;
      return res.str();
    }
    throw std::runtime_error{"not found"};
  });
});