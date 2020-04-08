#include "ChakraCommon.h"
#include "Core/mce.h"
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
};

struct OfflinePlayerBinding {
  Mod::OfflinePlayerEntry entry;
  OfflinePlayerBinding(Mod::OfflinePlayerEntry entry) : entry(entry) {}

  std::string GetXUID() { return std::to_string(entry.xuid); }
  std::string GetUUID() { return entry.uuid.asString(); }
  std::string GetNAME() { return entry.name; }
};

static ModuleRegister reg("ez:player", [](JsObjectWarpper native) -> std::string {
  JsObjectWarpper PlayerProto, OfflinePlayerProto;
  PlayerProto.DefineProperty("xuid"_jsp, {&PlayerBinding::GetXUID});
  PlayerProto.DefineProperty("uuid"_jsp, {&PlayerBinding::GetUUID});
  PlayerProto.DefineProperty("name"_jsp, {&PlayerBinding::GetNAME});
  PlayerProto.DefineProperty("address"_jsp, {&PlayerBinding::GetADDRESS});
  PlayerProto.DefineProperty("alive"_jsp, {&PlayerBinding::alive});

  OfflinePlayerProto.DefineProperty("xuid"_jsp, {&OfflinePlayerBinding::GetXUID});
  OfflinePlayerProto.DefineProperty("uuid"_jsp, {&OfflinePlayerBinding::GetUUID});
  OfflinePlayerProto.DefineProperty("name"_jsp, {&OfflinePlayerBinding::GetNAME});

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
  return R"(
    export const getPlayerByXUID = import.meta.native.getPlayerByXUID;
    export const getPlayerByUUID = import.meta.native.getPlayerByUUID;
    export const getPlayerByNAME = import.meta.native.getPlayerByNAME;

    export const getOfflinePlayerByXUID = import.meta.native.getOfflinePlayerByXUID;
    export const getOfflinePlayerByUUID = import.meta.native.getOfflinePlayerByUUID;
    export const getOfflinePlayerByNAME = import.meta.native.getOfflinePlayerByNAME;
  )";
});