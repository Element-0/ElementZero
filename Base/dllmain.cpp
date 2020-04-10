#include <iostream>
#include <filesystem>
#include <thread>
#include <fstream>
#include <iterator>

#include <base.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <Core/Minecraft.h>

#include <hook.h>
#include <log.h>
#include <yaml.h>
#include <dllentry.h>

#include "settings.hpp"
#include "loader.h"

Settings settings;

class DedicatedServer {
public:
  __declspec(dllexport) void asyncStop() { *(((bool *) this) + 48) = true; }
};

static DedicatedServer *mDedicatedServer = nullptr;
static RakNet::RakPeer *mRakPeer         = nullptr;
static AppPlatform *mAppPlatform         = nullptr;
std::string session;

mce::UUID const &SessionUUID() {
  if (!session.empty()) {
    static auto cached = mce::UUID::fromString(session);
    return cached;
  }
  return mce::UUID::EMPTY;
}

template <> DedicatedServer *LocateService<DedicatedServer>() { return mDedicatedServer; }
template <> ServerInstance *LocateService<ServerInstance>() {
  static auto ptr =
      GetServerSymbol<ServerInstance *>("?mService@?$ServiceLocator@VServerInstance@@@@0PEAVServerInstance@@EA");
  return *ptr;
}
template <> Level *LocateService<Level>() { return LocateService<Minecraft>()->getLevel(); }
template <> RakNet::RakPeer *LocateService<RakNet::RakPeer>() { return mRakPeer; }
template <> AppPlatform *LocateService<AppPlatform>() { return mAppPlatform; }

TClasslessInstanceHook(void, "?initialize@AppPlatform@@QEAAXXZ") { mAppPlatform = (AppPlatform *) this; }

TInstanceHook(
    int,
    "?start@DedicatedServer@@QEAA?AW4StartResult@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
    DedicatedServer, std::string const &uuid) {
  mDedicatedServer = this;
  session          = uuid;
  return original(this, uuid);
}

TInstanceHook(
    void,
    "?announceServer@RakNetServerLocator@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@"
    "0W4GameType@@HH_N@Z",
    FakeClass, std::string const &name, std::string const &desc, int type, int player, int max_players, bool flag) {
  *(RakNet::RakPeer **) ((char *) this + 280) = mRakPeer;
  original(this, name, desc, type, player, max_players, flag);
  *(RakNet::RakPeer **) ((char *) this + 280) = nullptr;
}

THook(void, "??0RakPeer@RakNet@@QEAA@XZ", RakNet::RakPeer *self) {
  if (!mRakPeer) mRakPeer = self;
  original(self);
}

static bool stopping = false;

THook(void *, "??R?$default_delete@VConsoleInputReader@@@std@@QEBAXPEAVConsoleInputReader@@@Z", void *self, char *s) {
  auto &thrd = *((std::thread *) (s + 88));
  if (thrd.joinable()) thrd.detach();
  auto ret = original(self, s);
  return ret;
}

static BOOL ConsoleCtrlHandler(DWORD type) {
  DEF_LOGGER("ConsoleCtrlHandler");
  if (type == CTRL_C_EVENT) {
    if (mDedicatedServer) {
      if (stopping) return TRUE;
      mDedicatedServer->asyncStop();
      LOGW("Requested to stop");
      stopping = true;
      return TRUE;
    }
    ExitProcess(0);
  }
  return TRUE;
}

static YAML::Node readConfig() {
  constexpr auto config_name = "custom.yaml";
  try {
    return YAML::LoadFile(config_name);
  } catch (YAML::BadFile const &e) {
    YAML::Emitter out;
    out.SetIndent(2);
    YAML::Node node;
    WriteYAML(settings, node);
    out << node;
    std::ofstream{config_name} << out.c_str();
    return node;
  }
}

static void writeConfig(YAML::Node const &node) {
  constexpr auto config_name = "custom.yaml";
  YAML::Emitter emitter;
  emitter.SetIndent(2);
  emitter << node;
  std::ofstream{config_name} << emitter.c_str();
}

class MBuf : public std::stringbuf {
public:
  int sync() {
    fputs(str().c_str(), stdout);
    str("");
    return 0;
  }
} buf;

void dllenter() {
  using namespace std::filesystem;
  DEF_LOGGER("Base");
  SetDllDirectory(L"Mods");
  SetConsoleCP(65001);
  SetConsoleOutputCP(65001);
  SetConsoleMode(
      GetStdHandle(STD_OUTPUT_HANDLE),
      ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  std::cout.rdbuf(&buf);

  std::thread::id this_id = std::this_thread::get_id();
  LOGV("Current thread id: %d") % this_id;
  LOGI("Base mod loaded, setting up CtrlC handler...");
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  try {
    auto cfg     = readConfig();
    bool changed = !ReadYAML(settings, cfg);
    if (changed) WriteYAML(settings, cfg);
    auto mods = cfg["mods"];
    initDatabase();
    if (settings.ModEnabled) loadMods(mods);
    writeConfig(cfg);
  } catch (std::exception const &e) { LOGE("Unexcepted exception: %s") % e.what(); }
}
void dllexit() {}