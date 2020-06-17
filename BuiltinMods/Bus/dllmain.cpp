#include <dllentry.h>
#include <base/base.h>
#include <base/log.h>
#include "global.h"

DEF_LOGGER("Bus");
DEFAULT_SETTINGS(settings);

static mini_bus::io_service service;

std::map<std::string, void (*)()> &RegisterAPI::GetMap() {
  static std::map<std::string, void (*)()> temp;
  return temp;
}
std::list<std::pair<std::string, void (*)()>> &RegisterAPI::GetPreloadList() {
  static std::list<std::pair<std::string, void (*)()>> temp;
  return temp;
}

void ServerStart() {
  LOGI("Connecting...");
  client = std::make_unique<mini_bus::MiniBusClient>(
      service, mini_bus::ip::address::from_string(settings.host), settings.port);
  LOGI("Connected");
  {
    auto temp = std::move(RegisterAPI::GetPreloadList());
    for (auto [name, fn] : temp) {
      LOGV("Load extension for %s") % name;
      fn();
    }
  }
  {
    auto temp = std::move(RegisterAPI::GetMap());
    for (auto [name, fn] : temp) {
      if (GetLoadedMod(name.data())) {
        LOGV("Load builtin extension for %s") % name;
        fn();
      } else {
        LOGV("Skip builtin extension for %s: Target mod not loaded") % name;
      }
    }
  }
  client->set("registry", settings.name, "");
}

void BeforeUnload() { client.reset(); }

namespace Mod::Bus {

void PostInitAction(std::string const &name, void (*fn)()) {
  if (client)
    fn();
  else
    RegisterAPI::GetPreloadList().emplace_back(name, fn);
}

void AddMethod(std::string const &key, std::function<std::string(std::string_view)> fn) {
  client->register_handler(key, fn);
}

void Broadcast(std::string_view key, std::string_view data) { client->notify(key, data); }

std::string Call(std::string_view bucket, std::string_view key, std::string_view data) {
  return client->call(bucket, key, data);
}

void SetACL(std::string_view key, ACL acl) { client->acl(key, (mini_bus::MiniBusClient::ACL) acl); }

std::list<std::string> ListKeys(std::string_view bucket) {
  std::list<std::string> ret;
  for (auto &[a, t] : client->keys(bucket)) { ret.emplace_back(t); }
  return ret;
}

void SetKey(std::string_view key, std::string_view data) { client->set_private(key, data); }
void SetKey(std::string_view bucket, std::string_view key, std::string_view data) { client->set(bucket, key, data); }

std::string GetKey(std::string_view key) { return client->get_private(key); }
std::string GetKey(std::string_view bucket, std::string_view key) { return client->get(bucket, key); }

void DelKey(std::string_view key) { client->del_private(key); }
void DelKey(std::string_view bucket, std::string_view key) { client->del(bucket, key); }

} // namespace Mod::Bus