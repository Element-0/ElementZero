#include <mods/ScriptSupport/chakra_helper.h>
#include <mods/ChatAPI.h>

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
  native["sendAnnounce"] = +[](std::string const &content) {
    Mod::Chat::GetInstance().SendAnnounce(content);
    return GetUndefined();
  };
  native["send"] = +[](Mod::PlayerEntry const &target, std::string const &content) {
    Mod::Chat::GetInstance().Send(target, content);
    return GetUndefined();
  };
  return R"script(
    export const onChat = import.meta.native.onChat;
    export const sendBroadcast = import.meta.native.sendBroadcast;
    export const sendAnnounce = import.meta.native.sendAnnounce;
    export const send = import.meta.native.send;
  )script";
});