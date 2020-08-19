#include <Core/ServerInstance.h>

#include <base/playerdb.h>
#include <base/base.h>
#include <mods/ChatAPI.h>

#include "global.h"

void ChatHandler(
    Mod::PlayerEntry const &entry, std::string &displayName, std::string &content,
    Mod::CallbackToken<std::string> &token) try {
  client->notify("chat", displayName + "\n" + content);
} catch (...) {}

static RegisterAPI reg("ChatAPI", true, [] {
  static auto _ [[gnu::unused]] = IIFE([] {
    Mod::Chat::GetInstance().AddListener(SIG("chat"), {Mod::RecursiveEventHandlerAdaptor(ChatHandler)});
    return 0;
  });
  client->register_handler("announce", [](std::string_view content) -> std::string {
    Mod::Chat::GetInstance().SendAnnounce(std::string{content});
    return "";
  });
});