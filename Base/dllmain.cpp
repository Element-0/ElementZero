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
static std::string session;

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

struct FakeClass {
  inline FakeClass() {}
  inline ~FakeClass() {}
};

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

enum class LogArea {};
enum class LogAreaID : unsigned char {
  ALL,
  PLATFORM,
  ENTITY,
  DATABASE,
  GUI,
  SYSTEM,
  NETWORK,
  RENDER,
  MEMORY,
  ANIMATION,
  INPUT,
  LEVEL,
  SERVER,
  DLC,
  PHYSICS,
  FILE,
  STORAGE,
  REALMS,
  REALMSAPI,
  XBOXLIVE,
  USERMANAGER,
  XSAPI,
  PERF,
  TELEMETRY,
  BLOCKS,
  RAKNET,
  GAMEFACE,
  SOUND,
  INTERACTIVE,
  SCRIPTING,
  PLAYFAB,
  AUTOMATION,
  PERSONA,
  MODDING
};

class BedrockLog {
public:
  class LogDetails {
    __declspec(dllimport) void _logToFile(std::string const &);

  public:
    void proxy(std::string const &data) { _logToFile(data); }
  };

  __declspec(dllexport) static char const *_areaFilterString(LogAreaID id) {
    switch (id) {
    case LogAreaID::ALL: return "ALL";
    case LogAreaID::PLATFORM: return "PLATFORM";
    case LogAreaID::ENTITY: return "ENTITY";
    case LogAreaID::DATABASE: return "DATABASE";
    case LogAreaID::GUI: return "GUI";
    case LogAreaID::SYSTEM: return "SYSTEM";
    case LogAreaID::NETWORK: return "NETWORK";
    case LogAreaID::RENDER: return "RENDER";
    case LogAreaID::MEMORY: return "MEMORY";
    case LogAreaID::ANIMATION: return "ANIMATION";
    case LogAreaID::INPUT: return "INPUT";
    case LogAreaID::LEVEL: return "LEVEL";
    case LogAreaID::SERVER: return "SERVER";
    case LogAreaID::DLC: return "DLC";
    case LogAreaID::PHYSICS: return "PHYSICS";
    case LogAreaID::FILE: return "FILE";
    case LogAreaID::STORAGE: return "STORAGE";
    case LogAreaID::REALMS: return "REALMS";
    case LogAreaID::REALMSAPI: return "REALMSAPI";
    case LogAreaID::XBOXLIVE: return "XBOXLIVE";
    case LogAreaID::USERMANAGER: return "USERMANAGER";
    case LogAreaID::XSAPI: return "XSAPI";
    case LogAreaID::PERF: return "PERF";
    case LogAreaID::TELEMETRY: return "TELEMETRY";
    case LogAreaID::BLOCKS: return "BLOCKS";
    case LogAreaID::RAKNET: return "RAKNET";
    case LogAreaID::GAMEFACE: return "GAMEFACE";
    case LogAreaID::SOUND: return "SOUND";
    case LogAreaID::INTERACTIVE: return "INTERACTIVE";
    case LogAreaID::SCRIPTING: return "SCRIPTING";
    case LogAreaID::PLAYFAB: return "PLAYFAB";
    case LogAreaID::AUTOMATION: return "AUTOMATION";
    case LogAreaID::PERSONA: return "PERSONA";
    case LogAreaID::MODDING: return "MODDING";
    default: return "UNKNOWN";
    }
  }

  __declspec(dllexport) static char const *_priorityFilterString(unsigned int id) {
    switch (id) {
    case 0xFFFFFFFF: return "ALL";
    case 1: return "VERBOSE";
    case 2: return "INFO";
    case 4: return "WARN";
    case 8: return "ERROR";
    default: return "UNKNOWN";
    }
  }
};

class ContentLog {
public:
  __declspec(dllimport) static char const *getLogAreaName(LogArea);
};

static char getPriority(unsigned int pri) {
  switch (pri) {
  case 1: return 'V';
  case 2: return 'I';
  case 4: return 'W';
  case 8: return 'E';
  default: return 'U';
  }
}

static std::unique_ptr<SQLite::Database> log_database;
static BedrockLog::LogDetails *log_instance;

TInstanceHook(void, "?_openLogFile@LogDetails@BedrockLog@@AEAAXXZ", BedrockLog::LogDetails) {
  original(this);
  log_instance = this;
}

void generalLog(unsigned int pri, std::string_view area, char const *source, unsigned line, std::string content) {
  auto f    = boost::format(settings.LogSettings.Format) % getPriority(pri) % area % source % line % content;
  auto data = f.str();
  if (data.back() != '\n') data.append("\n");
  std::cout << data;
  if (log_instance) log_instance->proxy(data);
  if (log_database) {
    try {
      static SQLite::Statement insert{*log_database,
                                      "INSERT INTO log (session, time, priority, area, source, line, content) "
                                      "VALUES (?, date('now'), ?, ?, ?, ?, ?)"};
      if (session != "") {
        insert.bindNoCopy(1, session);
        insert.bind(2, pri);
        insert.bindNoCopy(3, area.data());
        insert.bindNoCopy(4, source);
        insert.bind(5, line);
        insert.bindNoCopy(6, data);
        insert.exec();
        insert.reset();
        insert.clearBindings();
      }
    } catch (std::exception &ex) {
      log_database.reset();
      std::cerr << ex.what() << std::endl;
    }
  }
}

void Mod::Logger::commit(Level level, unsigned line, std::string value) {
  generalLog((unsigned) level, name, source, line, value);
}

TInstanceHook(
    void, "?_log_va@LogDetails@BedrockLog@@AEAAXW4LogAreaID@@IPEBDHH1PEAD@Z", BedrockLog::LogDetails, LogAreaID id,
    unsigned pri, char const *source, int line, int index, char const *format, va_list vargs) {
  static char buffer[65536];
  vsprintf_s(buffer, format, vargs);
  generalLog(pri, BedrockLog::_areaFilterString(id), source, line, buffer);
}

static bool stopping = false;

THook(void *, "??R?$default_delete@VConsoleInputReader@@@std@@QEBAXPEAVConsoleInputReader@@@Z", void *self, char *s) {
  ((std::thread *) (s + 88))->detach();
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
    out << YAML::BeginMap;
    out << YAML::Key << "mod-enabled";
    out << YAML::Value << true;
    out << YAML::EndMap;
    std::ofstream{config_name} << out.c_str();
    return YAML::LoadFile(config_name);
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

  std::thread::id this_id = std::this_thread::get_id();
  LOGV("Current thread id: %d") % this_id;
  LOGI("Base mod loaded, setting up CtrlC handler...");
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  std::list<PrePostInitType> PostInits;
  try {
    auto cfg     = readConfig();
    bool changed = false;
    yaml_assign(settings, cfg);
    if (settings.LogSettings.Database != "") {
      log_database = std::make_unique<SQLite::Database>(
          settings.LogSettings.Database, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
      log_database->exec(
          "CREATE TABLE IF NOT EXISTS log (session TEXT, time NUMERIC, priority INTEGER, area TEXT, source TEXT, line "
          "INTEGER, content TEXT)");
      // You don't need data safety for log
      log_database->exec("PRAGMA journal_mode = WAL");
      log_database->exec("PRAGMA synchronous = NORMAL");
      LOGV("Log data base initialized (location %s)") % settings.LogSettings.Database;
    }
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