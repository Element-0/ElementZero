#include "pch.h"

class DedicatedServer {
public:
  __declspec(dllexport) void asyncStop() { *(((bool *) this) + 48) = true; }
};

static DedicatedServer *mDedicatedServer = nullptr;

DedicatedServer *GetDedicatedServer() { return mDedicatedServer; }

TInstanceHook(
  int,
  ?start@DedicatedServer@@QEAA?AW4StartResult@1@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z,
  DedicatedServer,
  std::string const &uuid) {
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
    out.SetMapFormat(YAML::Flow);
    out << YAML::BeginMap;
    out << YAML::Key << "mod_enabled";
    out << YAML::Value << true;
    out << YAML::EndMap;
    std::ofstream{config_name} << out.c_str();
    return YAML::LoadFile(config_name);
  }
}

typedef void (*ApplySettingsType)(YAML::Node const &);

static void entry() {
  using namespace std::filesystem;

  std::thread::id this_id = std::this_thread::get_id();
  std::wcout << L"Current thread id: " << this_id << std::endl;
  std::wcout << L"Base mod loaded, setting up CtrlC handler..." << std::endl;
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  try {
    const auto cfg = readConfig();
    if (cfg["mod_enabled"].as<bool>()) {
      std::error_code ec;
      for (directory_iterator next("Mods", directory_options::follow_directory_symlink, ec), end; next != end; ++next) {
        if (next->is_regular_file() && next->path().extension() == ".dll") {
          const auto cfgkey = "mod_" + boost::algorithm::to_lower_copy(next->path().stem().string());
          const auto subcfg = cfg[cfgkey];
          if (subcfg)
            if (auto enabled = subcfg["enabled"]; enabled && !enabled.as<bool>()) continue;

          auto lib = LoadLibrary(next->path().c_str());
          if (!lib) std::wcout << L"Error: Failed to load mod: " << next->path() << std::endl;
          if (subcfg)
            if (auto fn = (ApplySettingsType) GetProcAddress(lib, "ApplySettings"); fn) fn(subcfg);
          std::wcout << L"Loaded mod " << canonical(next->path()) << std::endl;
        }
      }
      if (ec) std::cout << "Warning: Cannot open Mods folder: " << ec.message() << std::endl;
    }
  } catch (std::exception const &e) { std::wcerr << e.what() << std::endl; }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: entry(); break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH: break;
  }
  return TRUE;
}