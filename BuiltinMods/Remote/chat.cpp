#include <memory>
#include <stdexcept>

#include <flatbuffers/flatbuffers.h>

#include <chat.h>
#include <base.h>
#include <log.h>
#include <remote.h>

#include <Core/ServerInstance.h>

#include "global.h"

#include "proto/chat_event_generated.h"
#include "proto/chat_generated.h"
#include "proto/uuid_generated.h"

DEF_LOGGER("RemoteChat");

void ChatHandler(
    Mod::PlayerEntry const &entry, std::string &displayName, std::string &content,
    Mod::CallbackToken<std::string> &token);

static RegisterAPI reg("ChatAPI", true, [] {
  auto &chat = Mod::Chat::GetInstance();
  chat.AddListener(SIG("chat"), {Mod::RecursiveEventHandlerAdaptor(ChatHandler)});
  Mod::Remote::GetInstance().AddMethod("send_broadcast", [&](WsGw::BufferView view) -> WsGw::Buffer {
    flatbuffers::Verifier verifier{view.data(), view.size()};
    auto chat = Mod::proto::GetChat(view.data());
    if (!chat->Verify(verifier)) throw std::runtime_error{"verifier failed"};
    LocateService<ServerInstance>()->queueForServerThread(
        [name = chat->displayName()->str(), content = chat->content()->str()] {
          Mod::Chat::GetInstance().SendBroadcast(name, content);
        });
    return {};
  });
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