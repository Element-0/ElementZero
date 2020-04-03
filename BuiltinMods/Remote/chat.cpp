#include <chat.h>
#include <log.h>
#include <memory>

#include "global.h"

#include "proto/chat_event_generated.h"
#include "proto/uuid_generated.h"

DEF_LOGGER("RemoteChat");

void ChatHandler(
    Mod::PlayerEntry const &entry, std::string &displayName, std::string &content,
    Mod::CallbackToken<std::string> &token);

void InitChatHook() {
  auto &chat = Mod::Chat::GetInstance();
  chat.AddListener(SIG("chat"), {Mod::RecursiveEventHandlerAdaptor(ChatHandler)});
}

void ChatHandler(
    Mod::PlayerEntry const &entry, std::string &displayName, std::string &content,
    Mod::CallbackToken<std::string> &token) {
  flatbuffers::FlatBufferBuilder builder;
  Mod::proto::ChatEventT event;
  event.from        = entry.uuid;
  event.displayName = displayName;
  event.content     = content;
  builder.Finish(Mod::proto::ChatEvent::Pack(builder, &event));
  state->srv.Broadcast("chat", builder);
}