#include "loader.h"
#include "settings.hpp"

#include <filesystem>
#include <list>
#include <unordered_set>

#include <Windows.h>
#include <Psapi.h>

#include <log.h>
#include <hook.h>

using namespace std::filesystem;

DEF_LOGGER("ModLoader");

typedef void (*ApplySettingsType)(YAML::Node const &);
typedef bool (*GenerateSettingsType)(YAML::Node &);
typedef void (*PrePostInitType)();
typedef void (*WorldInitType)(std::filesystem::path const &);
typedef void (*BeforeUnloadType)();

class lc_string {
  std::string data;

public:
  operator std::string &() { return data; }
  operator std::string const &() const { return data; }

  lc_string(char const *init) { std::transform(init, init + strlen(init), std::back_inserter(data), &::tolower); }
  lc_string(std::string const &inp) { std::transform(inp.begin(), inp.end(), std::back_inserter(data), &::tolower); }

  bool operator<(lc_string const &rhs) const { return data < rhs.data; }
};

using lc_set = std::set<lc_string>;

struct ModLibrary {
  lc_string name;
  std::string keyname;
  HMODULE handle;

  ModLibrary(lc_string str) : name(str) {}

  ApplySettingsType applySettings;
  GenerateSettingsType generateSettings;
  PrePostInitType preInit, postInit;
  WorldInitType worldInit;
  BeforeUnloadType beforeUnload;

  lc_set dependencies;
};

static std::list<ModLibrary> LibList;
static std::list<PrePostInitType> PostInits;
static std::list<WorldInitType> WorldInits;
static std::list<BeforeUnloadType> UnloadHooks;
static lc_set LibNameList;

static void doLoadLib(YAML::Node &cfg_node, ModLibrary const &lib);

void loadMods(YAML::Node &cfg_node) {
  std::error_code ec;
  for (directory_iterator next("Mods", directory_options::follow_directory_symlink, ec), end; next != end; ++next) {
    if (next->is_regular_file() && next->path().extension() == ".dll") {
      const auto cfgkey = next->path().stem().string();
      auto subcfg       = cfg_node[cfgkey];
      if (subcfg.IsMap()) {
        auto enabled = subcfg["enabled"];
        if (!enabled) {
          enabled = settings.ModDefaultEnabled;
          if (!settings.ModDefaultEnabled) continue;
        } else if (!enabled.as<bool>())
          continue;
      } else {
        subcfg["enabled"] = settings.ModDefaultEnabled;
        if (!settings.ModDefaultEnabled) continue;
      }
      auto handle = LoadLibrary(next->path().c_str());
      if (!handle) {
        LOGE("Failed to load mod: %s") % next->path();
        continue;
      }
      auto name = next->path().filename().string();
      ModLibrary lib{name};
      lib.keyname          = cfgkey;
      lib.handle           = handle;
      lib.applySettings    = (ApplySettingsType) GetProcAddress(handle, "ApplySettings");
      lib.generateSettings = (GenerateSettingsType) GetProcAddress(handle, "GenerateSettings");
      lib.preInit          = (PrePostInitType) GetProcAddress(handle, "PreInit");
      lib.postInit         = (PrePostInitType) GetProcAddress(handle, "PostInit");
      lib.worldInit        = (WorldInitType) GetProcAddress(handle, "WorldInit");
      lib.beforeUnload     = (BeforeUnloadType) GetProcAddress(handle, "BeforeUnload");
      LibList.emplace_back(lib);
      LibNameList.emplace(name);
      LOGV("Loaded %s") % absolute(next->path()).string();
    }
  }
  if (ec) {
    LOGE("Warning: Cannot open Mods folder: %s") % ec.message();
    return;
  }
  auto curproc = GetCurrentProcess();
  for (auto &lib : LibList) {
    MODULEINFO modinfo;
    GetModuleInformation(curproc, lib.handle, &modinfo, sizeof modinfo);

    auto dllbase         = (char *) modinfo.lpBaseOfDll;
    auto dos_header      = (PIMAGE_DOS_HEADER) dllbase;
    auto nt_header       = (PIMAGE_NT_HEADERS)(dllbase + dos_header->e_lfanew);
    auto optional_header = &nt_header->OptionalHeader;
    auto import_desc     = (PIMAGE_IMPORT_DESCRIPTOR)(
        dllbase + optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (auto impit = import_desc; impit->Characteristics; impit++) {
      lc_string name = (char const *) (dllbase + impit->Name);
      if (LibNameList.count(name) != 0) lib.dependencies.emplace(name);
    }
  }
  while (!LibList.empty()) {
    lc_set cache;
    auto it = LibList.begin();
    while (it != LibList.end()) {
      if (it->dependencies.empty()) {
        doLoadLib(cfg_node, *it);
        cache.emplace(it->name);
        it = LibList.erase(it);
      } else
        ++it;
    }
    for (auto &lib : LibList) {
      lc_set temp;
      std::set_difference(
          lib.dependencies.begin(), lib.dependencies.end(), cache.begin(), cache.end(),
          std::inserter(temp, temp.begin()));
      lib.dependencies = temp;
    }
  }
  for (auto hook : PostInits) hook();
}

void doLoadLib(YAML::Node &cfg_node, ModLibrary const &lib) {
  auto subcfg = cfg_node[lib.keyname];
  LOGI("Init %s") % lib.keyname;
  if (subcfg.IsMap()) {
    if (lib.applySettings) lib.applySettings(subcfg);
  }
  subcfg["enabled"] = true;
  if (lib.generateSettings) lib.generateSettings(subcfg);
  if (lib.preInit) lib.preInit();
  if (lib.postInit) PostInits.emplace_back(lib.postInit);
  if (lib.worldInit) WorldInits.emplace_back(lib.worldInit);
  if (lib.beforeUnload) UnloadHooks.emplace_back(lib.beforeUnload);
}

TClasslessInstanceHook(void, "?leaveGameSync@ServerInstance@@QEAAXXZ") {
  for (auto hook : UnloadHooks) { hook(); }
  original(this);
}

void worldHook(std::filesystem::path const &path) {
  for (auto init : WorldInits) init(path);
}