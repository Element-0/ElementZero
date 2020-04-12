#include <boost/lexical_cast.hpp>

#include <Core/mce.h>

#include <scriptapi.h>
#include <blacklist.h>

#include "../lazyapi.h"

using namespace Mod::Scripting;

static LazyModuleRegister reg("ez:blacklist", "Blacklist", [](JsObjectWarpper native) -> std::string {
  native["addByXUID"] = +[](std::string const &xuid_str, std::string const &reason, std::string const &op) {
    auto xuid = boost::lexical_cast<uint64_t>(xuid_str);
    Mod::Blacklist::GetInstance().Add(Mod::Blacklist::XUID(xuid), reason, op);
  };
  native["removeByXUID"] = +[](std::string const &xuid_str) {
    auto xuid = boost::lexical_cast<uint64_t>(xuid_str);
    Mod::Blacklist::GetInstance().Remove(Mod::Blacklist::XUID(xuid));
  };
  native["addByUUID"] = +[](std::string const &uuid_str, std::string const &reason, std::string const &op) {
    auto uuid = mce::UUID::fromString(uuid_str);
    Mod::Blacklist::GetInstance().Add(Mod::Blacklist::UUID(uuid), reason, op);
  };
  native["removeByUUID"] = +[](std::string const &uuid_str) {
    auto uuid = mce::UUID::fromString(uuid_str);
    Mod::Blacklist::GetInstance().Remove(Mod::Blacklist::UUID(uuid));
  };
  native["addByNAME"] = +[](std::string const &name, std::string const &reason, std::string const &op) {
    Mod::Blacklist::GetInstance().Add(Mod::Blacklist::NAME(name), reason, op);
  };
  native["removeByNAME"] =
      +[](std::string const &name) { Mod::Blacklist::GetInstance().Remove(Mod::Blacklist::NAME(name)); };
  return R"js(
    export const addByXUID = import.meta.native.addByXUID;
    export const removeByXUID = import.meta.native.removeByXUID;
    export const addByUUID = import.meta.native.addByUUID;
    export const removeByUUID = import.meta.native.removeByUUID;
    export const addByNAME = import.meta.native.addByNAME;
    export const removeByNAME = import.meta.native.removeByNAME;
  )js";
});