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

class DedicatedServer {
public:
  __declspec(dllexport) void asyncStop() { *(((bool *) this) + 48) = true; }
};

static DedicatedServer *mDedicatedServer = nullptr;

DedicatedServer *GetDedicatedServer() { return mDedicatedServer; }

TInstanceHook(
    int,
    "?start@DedicatedServer@@QEAA?AW4StartResult@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
    DedicatedServer, std::string const &uuid) {
  mDedicatedServer = this;
  return original(this, uuid);
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

static BedrockLog::LogDetails *log_instance;
static std::string format_pattern = "%1$c [%2$s] (%3$s:%4$d) %5$s";

TInstanceHook(void, "?_openLogFile@LogDetails@BedrockLog@@AEAAXXZ", BedrockLog::LogDetails) {
  original(this);
  log_instance = this;
}

void generalLog(unsigned int pri, std::string_view area, char const *source, unsigned line, std::string content) {
  auto f    = boost::format(format_pattern) % getPriority(pri) % area % source % line % content;
  auto data = f.str();
  if (data.back() != '\n') data.append("\n");
  std::cout << data;
  if (log_instance) log_instance->proxy(data);
}

void Mods::Logger::commit(Level level, unsigned line, std::string value) {
  generalLog((unsigned) level, name, source, line, value);
}

TInstanceHook(
    void, "?_log_va@LogDetails@BedrockLog@@AEAAXW4LogAreaID@@IPEBDHH1PEAD@Z", BedrockLog::LogDetails, LogAreaID id,
    unsigned pri, char const *source, int line, int index, char const *format, va_list vargs) {
  static char buffer[65536];
  vsprintf_s(buffer, format, vargs);
  generalLog(pri, BedrockLog::_areaFilterString(id), source, line, buffer);
}

static BOOL ConsoleCtrlHandler(DWORD type) {
  static bool stopping = false;
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
    out << YAML::Key << "format-pattern";
    out << YAML::Value << format_pattern;
    out << YAML::EndMap;
    std::ofstream{config_name} << out.c_str();
    return YAML::LoadFile(config_name);
  }
}

typedef void (*ApplySettingsType)(YAML::Node const &);

void dllenter() {
  using namespace std::filesystem;
  DEF_LOGGER("MODLOADER");

  std::thread::id this_id = std::this_thread::get_id();
  LOGV("Current thread id: %d") % this_id;
  LOGI("Base mod loaded, setting up CtrlC handler...");
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  try {
    const auto cfg = readConfig();
    if (auto p = cfg["format-pattern"]; p) format_pattern = p.as<std::string>();
    if (cfg["mod-enabled"].as<bool>()) {
      auto mods_config = cfg["mods"];
      std::error_code ec;
      for (directory_iterator next("Mods", directory_options::follow_directory_symlink, ec), end; next != end; ++next) {
        if (next->is_regular_file() && next->path().extension() == ".dll") {
          const auto cfgkey = next->path().stem().string();
          const auto subcfg = mods_config ? mods_config[cfgkey] : YAML::Node{};
          if (subcfg)
            if (auto enabled = subcfg["enabled"]; enabled && !enabled.as<bool>()) continue;

          auto lib = LoadLibrary(next->path().c_str());
          if (!lib) {
            LOGE("Failed to load mod: %s") % next->path();
            continue;
          }
          if (subcfg)
            if (auto fn = (ApplySettingsType) GetProcAddress(lib, "ApplySettings"); fn) {
              try {
                fn(subcfg);
              } catch (std::exception const &ex) {
                LOGE("Failed to apply settings for %s: %s") % canonical(next->path()) % ex.what();
              }
            }
          LOGI("Loaded mod %s") % canonical(next->path());
        }
      }
      if (ec) LOGE("Warning: Cannot open Mods folder: %s") % ec.message();
    }
  } catch (std::exception const &e) { LOGE("Unexcepted exception: %s") % e.what(); }
}
void dllexit() {}