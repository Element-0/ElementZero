#include <iostream>
#include <filesystem>
#include <thread>
#include <fstream>
#include <iterator>
#include <hook.h>
#include <base.h>
#include <log.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <yaml-cpp/yaml.h>
#include <dllentry.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <Core/Minecraft.h>
#include "settings.hpp"

class DedicatedServer {
public:
  __declspec(dllexport) void asyncStop() { *(((bool *) this) + 48) = true; }
};

static DedicatedServer *mDedicatedServer = nullptr;
static RakNet::RakPeer *mRakPeer         = nullptr;
std::string session;

template <> DedicatedServer *LocateService<DedicatedServer>() { return mDedicatedServer; }
template <> ServiceInstance *LocateService<ServiceInstance>() {
  static auto ptr =
      GetServerSymbol<ServiceInstance *>("?mService@?$ServiceLocator@VServerInstance@@@@0PEAVServerInstance@@EA");
  return *ptr;
}
template <> Level *LocateService<Level>() { return LocateService<Minecraft>()->getLevel(); }
template <> RakNet::RakPeer *LocateService<RakNet::RakPeer>() { return mRakPeer; }

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

typedef void (*ApplySettingsType)(YAML::Node const &);
typedef bool (*GenerateSettingsType)(YAML::Node &);
typedef void (*PrePostInitType)();
typedef void (*BeforeUnloadType)();

static std::list<BeforeUnloadType> UnloadHooks;

TClasslessInstanceHook(void, "?leaveGameSync@ServerInstance@@QEAAXXZ") {
  for (auto hook : UnloadHooks) { hook(); }
  original(this);
}

void dllenter() {
  using namespace std::filesystem;
  DEF_LOGGER("MODLOADER");
  SetDllDirectory(L"Mods");
  SetConsoleCP(65001);
  SetConsoleOutputCP(65001);
  SetConsoleMode(
      GetStdHandle(STD_OUTPUT_HANDLE),
      ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

  std::thread::id this_id = std::this_thread::get_id();
  LOGV("Current thread id: %d") % this_id;
  LOGI("Base mod loaded, setting up CtrlC handler...");
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  std::list<PrePostInitType> PostInits;
  try {
    auto cfg     = readConfig();
    bool changed = !ReadYAML(settings, cfg);
    if (changed) WriteYAML(settings, cfg);
    initDatabase();
    if (settings.ModEnabled) {
      auto mods_config = cfg["mods"];
      std::error_code ec;
      for (directory_iterator next("Mods", directory_options::follow_directory_symlink, ec), end; next != end; ++next) {
        if (next->is_regular_file() && next->path().extension() == ".dll") {
          const auto cfgkey = next->path().stem().string();
          auto subcfg       = settings.ModSettings[cfgkey];
          if (subcfg.IsMap())
            if (auto enabled = subcfg["enabled"]; enabled && !enabled.as<bool>()) continue;

          auto lib = LoadLibrary(next->path().c_str());
          if (!lib) {
            LOGE("Failed to load mod: %s") % next->path();
            continue;
          }
          if (subcfg.IsMap()) {
            if (auto fn = (ApplySettingsType) GetProcAddress(lib, "ApplySettings"); fn) {
              try {
                fn(subcfg);
              } catch (std::exception const &ex) {
                LOGE("Failed to apply settings for %s: %s") % canonical(next->path()) % ex.what();
              }
            }
          } else {
            if (auto fn = (GenerateSettingsType) GetProcAddress(lib, "GenerateSettings"); fn) fn(subcfg);
            mods_config[cfgkey] = subcfg;
            if (!changed) changed = subcfg.IsMap();
          }
          if (auto fn = (PrePostInitType) GetProcAddress(lib, "PreInit")) fn();
          if (auto fn = (PrePostInitType) GetProcAddress(lib, "PostInit")) PostInits.push_back(fn);
          if (auto fn = (BeforeUnloadType) GetProcAddress(lib, "BeforeUnload")) UnloadHooks.push_front(fn);
          LOGI("Loaded mod %s") % canonical(next->path());
        }
      }
      if (ec) LOGE("Warning: Cannot open Mods folder: %s") % ec.message();
    }
    if (changed) writeConfig(cfg);
    LOGV("Start post init");
    for (auto fn : PostInits) { fn(); }
  } catch (std::exception const &e) { LOGE("Unexcepted exception: %s") % e.what(); }
}
void dllexit() {}