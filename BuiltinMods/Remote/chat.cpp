#include <memory>

#include <chat.h>
#include <log.h>
#include <remote.h>

#include "global.h"

#include "proto/chat_event_generated.h"
#include "proto/uuid_generated.h"

DEF_LOGGER("RemoteChat");

void ChatHandler(
    Mod::PlayerEntry const &entry, std::string &displayName, std::string &content,
    Mod::CallbackToken<std::string> &token);

static RegisterAPI reg("ChatAPI", true, [] {
  auto &chat = Mod::Chat::GetInstance();
  chat.AddListener(SIG("chat"), {Mod::RecursiveEventHandlerAdaptor(ChatHandler)});
});

void ChatHandler(
    Mod::PlayerEntry const &entry, std::string &displayName, std::string &content,
    Mod::CallbackToken<std::string> &token) {
  static auto &instance = Mod::Remote::GetInstance();
  flatbuffers::FlatBufferBuilder builder;
  Mod::proto::ChatEventT event;
  event.from        = entry.uuid;
  event.displayName = displayName;
  event.content     = content;
  builder.Finish(Mod::proto::ChatEvent::Pack(builder, &event));
  instance.Broadcast("chat", builder);
}