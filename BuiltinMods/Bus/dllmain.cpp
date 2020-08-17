#include <chrono>
#include <memory>
#include <thread>

#include <dllentry.h>
#include <base/base.h>
#include <base/log.h>
#include "global.h"

DEF_LOGGER("Bus");
DEFAULT_SETTINGS(settings);

using namespace mini_bus;

static io_service service;

namespace {
struct MiniBusConnectionInfo : ConnectionInfo {
  std::optional<ip::tcp::socket> create_connected_socket() override {
    try {
      LOGV("try connecting");
      ip::tcp::socket ret{service};
      ret.connect(ip::tcp::endpoint{ip::address::from_string(settings.host), settings.port});
      LOGV("connected, handshaking");
      return {std::move(ret)};
    } catch (...) { return std::nullopt; }
  }
  void connected(MiniBusClient *) override {
    LOGV("connected, load extensions");
    for (auto [name, fn] : RegisterAPI::GetPreloadList()) {
      LOGV("Load extension for %s") % name;
      fn();
    }
    for (auto [name, fn] : RegisterAPI::GetMap()) {
      if (GetLoadedMod(name.data())) {
        LOGV("Load builtin extension for %s") % name;
        fn();
      } else {
        LOGV("Skip builtin extension for %s: Target mod not enabled") % name;
      }
    }
    client->set("registry", settings.name, "");
  }
  bool disconnected(bool imm) override {
    if (settings.reconnect_delay <= 0) { return false; }
    LOGW("disconnected, waiting for %d") % settings.reconnect_delay;
    std::this_thread::sleep_for(std::chrono::seconds(settings.reconnect_delay));
    return true;
  }
};
} // namespace

std::map<std::string, void (*)()> &RegisterAPI::GetMap() {
  static std::map<std::string, void (*)()> temp;
  return temp;
}
std::list<std::pair<std::string, void (*)()>> &RegisterAPI::GetPreloadList() {
  static std::list<std::pair<std::string, void (*)()>> temp;
  return temp;
}

void ServerStart() { client = std::make_unique<mini_bus::MiniBusClient>(std::make_shared<MiniBusConnectionInfo>()); }

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
  return *client->call(bucket, key, data)->wait();
}

void SetACL(std::string_view key, ACL acl) { client->acl(key, (mini_bus::MiniBusClient::ACL) acl); }

std::list<std::string> ListKeys(std::string_view bucket) {
  std::list<std::string> ret;
  for (auto &[a, t] : client->keys(bucket)->wait()) { ret.emplace_back(t); }
  return ret;
}

void SetKey(std::string_view key, std::string_view data) { client->set_private(key, data); }
void SetKey(std::string_view bucket, std::string_view key, std::string_view data) { client->set(bucket, key, data); }

std::string GetKey(std::string_view key) { return *client->get_private(key)->wait(); }
std::string GetKey(std::string_view bucket, std::string_view key) { return *client->get(bucket, key)->wait(); }

void DelKey(std::string_view key) { client->del_private(key); }
void DelKey(std::string_view bucket, std::string_view key) { client->del(bucket, key); }

} // namespace Mod::Bus