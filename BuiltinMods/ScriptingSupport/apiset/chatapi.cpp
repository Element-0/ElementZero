#include <chakra_helper.h>
#include <chat.h>

#include "../lazyapi.h"

using namespace Mod::Scripting;

static LazyModuleRegister reg("ez:chat", "ChatAPI", [](JsObjectWrapper native) -> std::string {
  native["onChat"] = +[](JsValueRef ref) {
    if (GetJsType(ref) != JsFunction) throw std::runtime_error{"Require function argument"};
    Mod::Chat::GetInstance().AddListener(
        SIG("chat"),
        {Mod::RecursiveEventHandlerAdaptor([fn{ValueHolder(ref)}](
                                               Mod::PlayerEntry const &entry, std::string &displayName,
                                               std::string &content, Mod::CallbackToken<std::string> &token) {
          if (token) return;
          JsObjectWrapper wrap;
          wrap["sender"]    = entry.name;
          wrap["content"]   = content;
          JsValueRef args[] = {GetUndefined(), *wrap};
          JsCallFunction(*fn, args, 2, nullptr);
        })});
    return GetUndefined();
  };
  native["sendBroadcast"] = +[](std::string const &sender, std::string const &content) {
    Mod::Chat::GetInstance().SendBroadcast(sender, content);
    return GetUndefined();
  };
  return R"script(
    export const onChat = import.meta.native.onChat;
    export const sendBroadcast = import.meta.native.sendBroadcast;
  )script";
});