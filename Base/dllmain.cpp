#include <iostream>
#include <filesystem>
#include <thread>
#include <fstream>
#include <iterator>
#include <hook.h>
#include <base.h>
#include <boost/algorithm/string.hpp>
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
  std::cout << "Server loading (" << uuid << ")..." << std::endl;
  return original(this, uuid);
}

static BOOL ConsoleCtrlHandler(DWORD type) {
  static bool stopping = false;
  if (type == CTRL_C_EVENT) {
    if (mDedicatedServer) {
      if (stopping) return TRUE;
      mDedicatedServer->asyncStop();
      std::wcout << L"Requested to stop" << std::endl;
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
    out << YAML::Key << "mod_enabled";
    out << YAML::Value << true;
    out << YAML::EndMap;
    std::ofstream{config_name} << out.c_str();
    return YAML::LoadFile(config_name);
  }
}

typedef void (*ApplySettingsType)(YAML::Node const &);

void dllenter() {
  using namespace std::filesystem;

  std::thread::id this_id = std::this_thread::get_id();
  std::wcout << L"Current thread id: " << this_id << std::endl;
  std::wcout << L"Base mod loaded, setting up CtrlC handler..." << std::endl;
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  try {
    const auto cfg = readConfig();
    if (cfg["mod_enabled"].as<bool>()) {
      auto mods_config = cfg["mods"];
      std::error_code ec;
      for (directory_iterator next("Mods", directory_options::follow_directory_symlink, ec), end; next != end; ++next) {
        if (next->is_regular_file() && next->path().extension() == ".dll") {
          const auto cfgkey = next->path().stem().string();
          const auto subcfg = mods_config ? mods_config[cfgkey] : YAML::Node{};
          if (subcfg)
            if (auto enabled = subcfg["enabled"]; enabled && !enabled.as<bool>()) continue;

          auto lib = LoadLibrary(next->path().c_str());
          if (!lib) std::wcout << L"Error: Failed to load mod: " << next->path() << std::endl;
          if (subcfg)
            if (auto fn = (ApplySettingsType) GetProcAddress(lib, "ApplySettings"); fn) {
              try {
                fn(subcfg);
              } catch (std::exception const &ex) {
                std::wcout << L"Failed to apply settings for " << canonical(next->path()) << std::endl;
                std::cerr << ex.what() << std::endl;
              }
            }
          std::wcout << L"Loaded mod " << canonical(next->path()) << std::endl;
        }
      }
      if (ec) std::cout << "Warning: Cannot open Mods folder: " << ec.message() << std::endl;
    }
  } catch (std::exception const &e) { std::wcerr << e.what() << std::endl; }
}
void dllexit() {}