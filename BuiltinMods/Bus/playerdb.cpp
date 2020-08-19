#include <stdexcept>

#include <boost/format.hpp>

#include <Core/ServerInstance.h>

#include <base/playerdb.h>
#include <base/base.h>

#include "global.h"

static RegisterAPI reg("ChatAPI", false, [] {
  static auto _ [[gnu::unused]] = IIFE([] {
    auto &playerdb = Mod::PlayerDatabase::GetInstance();
    playerdb.AddListener(SIG("joined"), [](Mod::PlayerEntry const &player) {
      auto res = boost::format("%s\n%s\n%s") % player.name % player.xuid % player.uuid;
      try {
        client->notify("player-join", res.str());
      } catch (...) {}
    });
    playerdb.AddListener(SIG("left"), [](Mod::PlayerEntry const &player) {
      auto res = boost::format("%s\n%s\n%s") % player.name % player.xuid % player.uuid;
      try {
        client->notify("player-join", res.str());
      } catch (...) {}
    });
    return 0;
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