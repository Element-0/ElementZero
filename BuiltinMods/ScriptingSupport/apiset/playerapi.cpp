#include <stdexcept>

#include "ChakraCommon.h"
#include "Core/mce.h"
#include "boost/lexical_cast.hpp"
#include "chakra_helper.h"
#include "log.h"
#include <playerdb.h>
#include <modutils.h>

#include <scriptapi.h>

using namespace Mod::Scripting;

JsObjectWarpper PlayerBinding::InitProto() {
  static JsObjectWarpper proto = IIFE([] {
    JsObjectWarpper PlayerProto;
    PlayerProto["xuid"]       = JsObjectWarpper::PropertyDesc{&PlayerBinding::GetXUID};
    PlayerProto["uuid"]       = JsObjectWarpper::PropertyDesc{&PlayerBinding::GetUUID};
    PlayerProto["name"]       = JsObjectWarpper::PropertyDesc{&PlayerBinding::GetNAME};
    PlayerProto["address"]    = JsObjectWarpper::PropertyDesc{&PlayerBinding::GetADDRESS};
    PlayerProto["alive"]      = JsObjectWarpper::PropertyDesc{&PlayerBinding::alive};
    PlayerProto["getOffline"] = &PlayerBinding::ToOffline;
    PlayerProto["toString"]   = &PlayerBinding::toString;
    return PlayerProto;
  });
  return proto;
}

JsObjectWarpper OfflinePlayerBinding::InitProto() {
  static JsObjectWarpper proto = IIFE([] {
    JsObjectWarpper OfflinePlayerProto;
    OfflinePlayerProto["xuid"]      = JsObjectWarpper::PropertyDesc{&OfflinePlayerBinding::GetXUID};
    OfflinePlayerProto["uuid"]      = JsObjectWarpper::PropertyDesc{&OfflinePlayerBinding::GetUUID};
    OfflinePlayerProto["name"]      = JsObjectWarpper::PropertyDesc{&OfflinePlayerBinding::GetNAME};
    OfflinePlayerProto["getOnline"] = &OfflinePlayerBinding::ToOnline;
    OfflinePlayerProto["toString"]  = &OfflinePlayerBinding::toString;
    return OfflinePlayerProto;
  });
  return proto;
}

static ModuleRegister reg("ez:player", [](JsObjectWarpper native) -> std::string {
  JsObjectWarpper PlayerProto = PlayerBinding::InitProto(), OfflinePlayerProto = OfflinePlayerBinding::InitProto();

  native["getPlayerByXUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto xuid = boost::lexical_cast<uint64_t>(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.Find(xuid); it) return ToJs(*it);
    return GetUndefined();
  };
  native["getPlayerByUUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto uuid = mce::UUID::fromString(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.Find(uuid); it) return ToJs(*it);
    return GetUndefined();
  };
  native["getPlayerByNAME"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto name = JsToString(args[0]);
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.Find(name); it) return ToJs(*it);
    return GetUndefined();
  };

  native["getOfflinePlayerByXUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto xuid = boost::lexical_cast<uint64_t>(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.FindOffline(xuid); it) return ToJs(*it);
    return GetUndefined();
  };
  native["getOfflinePlayerByUUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto uuid = mce::UUID::fromString(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.FindOffline(uuid); it) return ToJs(*it);
    return GetUndefined();
  };
  native["getOfflinePlayerByNAME"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto name = JsToString(args[0]);
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.FindOffline(name); it) return ToJs(*it);
    return GetUndefined();
  };

  native["getPlayerList"] = [=](JsValueRef callee, Arguments args) {
    auto &db = Mod::PlayerDatabase::GetInstance();
    return ToJsArray(db.GetData());
  };

  native["onPlayerJoined"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    if (GetJsType(args[0]) != JsFunction) throw std::runtime_error{"Require function argument"};
    auto &db = Mod::PlayerDatabase::GetInstance();
    db.AddListener(SIG("joined"), [=, fn = args[0]](Mod::PlayerEntry const &entry) {
      JsValueRef ar[] = {GetUndefined(), ToJs(entry)};
      JsValueRef res;
      JsCallFunction(fn, ar, 2, &res);
    });
    return GetUndefined();
  };
  native["onPlayerLeft"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    if (GetJsType(args[0]) != JsFunction) throw std::runtime_error{"Require function argument"};
    auto &db = Mod::PlayerDatabase::GetInstance();
    db.AddListener(SIG("left"), [=, fn = args[0]](Mod::PlayerEntry const &entry) {
      JsValueRef ar[] = {GetUndefined(), ToJs(entry)};
      JsValueRef res;
      JsCallFunction(fn, ar, 2, &res);
    });
    return GetUndefined();
  };
  return R"(
    export const getPlayerByXUID = import.meta.native.getPlayerByXUID;
    export const getPlayerByUUID = import.meta.native.getPlayerByUUID;
    export const getPlayerByNAME = import.meta.native.getPlayerByNAME;

    export const getOfflinePlayerByXUID = import.meta.native.getOfflinePlayerByXUID;
    export const getOfflinePlayerByUUID = import.meta.native.getOfflinePlayerByUUID;
    export const getOfflinePlayerByNAME = import.meta.native.getOfflinePlayerByNAME;

    export const getPlayerList = import.meta.native.getPlayerList;

    export const onPlayerJoined = import.meta.native.onPlayerJoined;
    export const onPlayerLeft = import.meta.native.onPlayerLeft;
  )";
});