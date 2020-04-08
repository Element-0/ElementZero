#include "ChakraCommon.h"
#include "Core/mce.h"
#include "boost/format/format_fwd.hpp"
#include "boost/lexical_cast.hpp"
#include "chakra_helper.h"
#include "log.h"
#include <playerdb.h>

#include <scriptapi.h>
#include <stdexcept>

struct PlayerBinding {
  Mod::PlayerEntry entry;

  PlayerBinding(Mod::PlayerEntry entry) : entry(entry) {}

  std::string GetXUID() { return std::to_string(entry.xuid); }
  std::string GetUUID() { return entry.uuid.asString(); }
  std::string GetNAME() { return entry.name; }
  std::string GetADDRESS() { return entry.netid.getRealAddress().ToString(); }
  bool alive() { return Mod::PlayerDatabase::GetInstance().Find(entry.xuid).has_value(); }

  std::string toString() {
    return (boost::format("Player { xuid: %d, uuid: %s, name: %s, ip: %s }") % entry.xuid % GetUUID() % GetNAME() %
            GetADDRESS())
        .str();
  }
};

struct OfflinePlayerBinding {
  Mod::OfflinePlayerEntry entry;
  OfflinePlayerBinding(Mod::OfflinePlayerEntry entry) : entry(entry) {}

  std::string GetXUID() { return std::to_string(entry.xuid); }
  std::string GetUUID() { return entry.uuid.asString(); }
  std::string GetNAME() { return entry.name; }

  std::string toString() {
    return (boost::format("OfflinePlayer { xuid: %d, uuid: %s, name: %s }") % entry.xuid % GetUUID() % GetNAME()).str();
  }
};

static ModuleRegister reg("ez:player", [](JsObjectWarpper native) -> std::string {
  JsObjectWarpper PlayerProto, OfflinePlayerProto;
  PlayerProto.DefineProperty("xuid"_jsp, {&PlayerBinding::GetXUID});
  PlayerProto.DefineProperty("uuid"_jsp, {&PlayerBinding::GetUUID});
  PlayerProto.DefineProperty("name"_jsp, {&PlayerBinding::GetNAME});
  PlayerProto.DefineProperty("address"_jsp, {&PlayerBinding::GetADDRESS});
  PlayerProto.DefineProperty("alive"_jsp, {&PlayerBinding::alive});
  PlayerProto["toString"] = &PlayerBinding::toString;

  OfflinePlayerProto.DefineProperty("xuid"_jsp, {&OfflinePlayerBinding::GetXUID});
  OfflinePlayerProto.DefineProperty("uuid"_jsp, {&OfflinePlayerBinding::GetUUID});
  OfflinePlayerProto.DefineProperty("name"_jsp, {&OfflinePlayerBinding::GetNAME});
  OfflinePlayerProto["toString"] = &OfflinePlayerBinding::toString;

  native["getPlayerByXUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto xuid = boost::lexical_cast<uint64_t>(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.Find(xuid); it) return *JsObjectWarpper::FromExternalObject(new PlayerBinding(*it), *PlayerProto);
    return GetUndefined();
  };
  native["getPlayerByUUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto uuid = mce::UUID::fromString(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.Find(uuid); it) return *JsObjectWarpper::FromExternalObject(new PlayerBinding(*it), *PlayerProto);
    return GetUndefined();
  };
  native["getPlayerByNAME"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto name = JsToString(args[0]);
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.Find(name); it) return *JsObjectWarpper::FromExternalObject(new PlayerBinding(*it), *PlayerProto);
    return GetUndefined();
  };

  native["getOfflinePlayerByXUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto xuid = boost::lexical_cast<uint64_t>(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.FindOffline(xuid); it)
      return *JsObjectWarpper::FromExternalObject(new OfflinePlayerBinding(*it), *OfflinePlayerProto);
    return GetUndefined();
  };
  native["getOfflinePlayerByUUID"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto uuid = mce::UUID::fromString(JsToString(args[0]));
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.FindOffline(uuid); it)
      return *JsObjectWarpper::FromExternalObject(new OfflinePlayerBinding(*it), *OfflinePlayerProto);
    return GetUndefined();
  };
  native["getOfflinePlayerByNAME"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    auto name = JsToString(args[0]);
    auto &db  = Mod::PlayerDatabase::GetInstance();
    if (auto it = db.FindOffline(name); it)
      return *JsObjectWarpper::FromExternalObject(new OfflinePlayerBinding(*it), *OfflinePlayerProto);
    return GetUndefined();
  };

  native["getPlayerList"] = [=](JsValueRef callee, Arguments args) {
    auto &db = Mod::PlayerDatabase::GetInstance();
    return ToJsArray(db.GetData(), [=](Mod::PlayerEntry const &ent) {
      return *JsObjectWarpper::FromExternalObject(new PlayerBinding(ent), *PlayerProto);
    });
  };

  native["onPlayerJoined"] = [=](JsValueRef callee, Arguments args) {
    if (args.size() != 1) throw std::runtime_error{"Require 1 argument"};
    if (GetJsType(args[0]) != JsFunction) throw std::runtime_error{"Require function argument"};
    auto &db = Mod::PlayerDatabase::GetInstance();
    db.AddListener(SIG("joined"), [=, fn = args[0]](Mod::PlayerEntry const &entry) {
      JsValueRef ar[] = {GetUndefined(), *JsObjectWarpper::FromExternalObject(new PlayerBinding(entry), *PlayerProto)};
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
      JsValueRef ar[] = {GetUndefined(), *JsObjectWarpper::FromExternalObject(new PlayerBinding(entry), *PlayerProto)};
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