#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>

#include <log.h>
#include <scriptapi.h>

#include "base.h"
#include "chakra_helper.h"
#include "global.h"
#include "lazyapi.h"

namespace fs = std::filesystem;

using namespace Mod::Scripting;

DEF_LOGGER("ScriptLoader");

static std::map<fs::path, JsModuleRecord> mod_cache;

static JsErrorCode FetchImportedModule(
    _In_ JsModuleRecord referencingModule, _In_ JsValueRef specifier, _Outptr_result_maybenull_ JsModuleRecord *target);

static JsErrorCode FetchImportedModuleFromScript(
    _In_ JsSourceContext dwReferencingSourceContext, _In_ JsValueRef specifier,
    _Outptr_result_maybenull_ JsModuleRecord *target);

static JsErrorCode
MyNotifyModuleReadyCallback(_In_opt_ JsModuleRecord referencingModule, _In_opt_ JsValueRef exceptionVar);
static JsErrorCode NativeInitializeImportMetaCallback(JsModuleRecord referencingModule, JsValueRef importMetaVar);

JsSourceContext NextContext() {
  static JsSourceContext ctx = 0;
  return ctx++;
}

static void LoadModuleFromFile(JsModuleRecord module, fs::path path, fs::path full) {
  auto cookies = NextContext();
  JsValueRef exception;

  JsValueRef real;
  auto str = path.native();
  ThrowError(JsCreateStringUtf16((uint16_t const *) str.c_str(), str.size(), &real));
  ThrowError(JsSetModuleHostInfo(module, JsModuleHostInfo_HostDefined, real));
  ThrowError(JsSetModuleHostInfo(module, JsModuleHostInfo_Url, real));

  std::ifstream ifs{full};
  if (!ifs) throw JsErrorInvalidArgument;
  std::string content{std::istreambuf_iterator{ifs}, {}};

  JsParseModuleSource(
      module, cookies, (BYTE *) content.data(), content.size(), JsParseModuleSourceFlags_DataIsUTF8, &exception);
}

static void initRootModule(std::string_view path, JsModuleRecord *requestModule) {
  JsValueRef specifier;

  ThrowError(JsCreateString(path.data(), path.size(), &specifier));
  ThrowError(JsInitializeModuleRecord(nullptr, specifier, requestModule));
  ThrowError(
      JsSetModuleHostInfo(*requestModule, JsModuleHostInfo_FetchImportedModuleCallback, (void *) FetchImportedModule));
  ThrowError(JsSetModuleHostInfo(
      *requestModule, JsModuleHostInfo_FetchImportedModuleFromScriptCallback, (void *) FetchImportedModuleFromScript));
  ThrowError(JsSetModuleHostInfo(
      *requestModule, JsModuleHostInfo_NotifyModuleReadyCallback, (void *) MyNotifyModuleReadyCallback));
  ThrowError(JsSetModuleHostInfo(
      *requestModule, JsModuleHostInfo_InitializeImportMetaCallback, (void *) &NativeInitializeImportMetaCallback));
  ThrowError(JsSetModuleHostInfo(*requestModule, JsModuleHostInfo_Url, specifier));
  ThrowError(JsSetModuleHostInfo(*requestModule, JsModuleHostInfo_HostDefined, specifier));
}

void loadCustomScript() try {
  JsModuleRecord root;

  if (!fs::exists("scripts/index.js")) {
    LOGW("scripts/index.js file not exists, skipping");
    return;
  }

  try {
    initRootModule("index.js", &root);
  } catch (JsErrorCode ec) {
    LOGE("Failed to init root module: %d") % ec;
    return;
  }
  LoadModuleFromFile(root, "index.js", "scripts/index.js");
  bool hasException;
  if (JsHasException(&hasException) != JsNoError) return;
  if (!hasException) return;
  try {
    auto metadata = JsObjectWrapper::FromCurrentException();
    LOGE("Exception %s") % metadata["exception"].ToString();
    LOGE("File: %s") % metadata["url"].ToString();
    LOGE("Line: %0.0f") % (metadata["line"].get<double>() + 1.0);
    LOGE("Column: %0.0f") % metadata["column"].get<double>();
  } catch (...) {}
  LOGE("Failed to load root module: %s") % hasException;
} catch (JsErrorCode ec) { LOGE("Failed to load: %d") % ec; }

static JsErrorCode resolveModule(JsValueRef specifier, fs::path &path, fs::path &full) {
  std::wstring addition = FromJs<std::wstring>(specifier);
  JsErrorCode ec        = JsNoError;

  path /= addition;
  path.make_preferred();
  path = fs::weakly_canonical(path);
  full = "scripts" / path;
  return ec;
}

static void LoadBuiltinModule(
    JsModuleRecord referencingModule, JsValueRef specifier, JsModuleRecord *target, std::string const &name,
    std::string (*fn)(JsObjectWrapper native)) {
  ThrowError(JsInitializeModuleRecord(referencingModule, specifier, target));
  ThrowError(JsSetModuleHostInfo(*target, JsModuleHostInfo_Url, specifier));
  auto cookies = NextContext();
  JsValueRef exception;
  JsObjectWrapper obj;
  const auto script = fn(obj);
  ThrowError(JsSetModuleHostInfo(*target, JsModuleHostInfo_HostDefined, obj.ref));
  ThrowError(JsParseModuleSource(
      *target, cookies, (BYTE *) script.data(), script.size(), JsParseModuleSourceFlags_DataIsUTF8, &exception));
  if (exception) JsSetException(exception);
  mod_cache[name] = *target;
}

static void FetchModule(JsModuleRecord referencingModule, JsValueRef specifier, JsModuleRecord *target) {
  DEF_LOGGER("FetchModule");
  fs::path base;
  fs::path full;

  auto &list     = ModuleRegister::GetList();
  auto &lazylist = LazyModuleRegister::GetList();

  auto name = FromJs<std::string>(specifier);

  LOGV("%s") % name;
  if (auto it = mod_cache.find(name); it != mod_cache.end()) {
    *target = it->second;
    return;
  }

  if (auto it = list.find(name); it != list.end()) {
    LoadBuiltinModule(referencingModule, specifier, target, name, it->second);
    return;
  }
  if (auto it = lazylist.find(name); it != lazylist.end()) {
    if (GetLoadedMod(it->second.first)) {
      LoadBuiltinModule(referencingModule, specifier, target, name, it->second.second);
      return;
    } else {
      throw JsErrorNotImplemented;
    }
  }

  if (referencingModule) {
    JsValueRef baseref;
    ThrowError(JsGetModuleHostInfo(referencingModule, JsModuleHostInfo_HostDefined, &baseref));
    std::wstring buf = FromJs<std::wstring>(baseref);
    base             = buf;
    base             = base.parent_path();
  }
  ThrowError(resolveModule(specifier, base, full));

  LOGV("try to load module %s") % base;

  if (auto it = mod_cache.find(base); it != mod_cache.end()) {
    *target = it->second;
    return;
  }

  ThrowError(JsInitializeModuleRecord(referencingModule, specifier, target));
  LoadModuleFromFile(*target, base, full);

  mod_cache[base] = *target;
}

static JsErrorCode FetchImportedModule(
    _In_ JsModuleRecord referencingModule, _In_ JsValueRef specifier,
    _Outptr_result_maybenull_ JsModuleRecord *target) try {
  FetchModule(referencingModule, specifier, target);
  return JsNoError;
} catch (JsErrorCode ec) {
  LOGE("Failed to fetch module %s: %d") % ToJs(specifier) % ec;
  return ec;
}

static JsErrorCode FetchImportedModuleFromScript(
    _In_ JsSourceContext dwReferencingSourceContext, _In_ JsValueRef specifier,
    _Outptr_result_maybenull_ JsModuleRecord *target) try {
  FetchModule(nullptr, specifier, target);
  return JsNoError;
} catch (JsErrorCode ec) {
  LOGE("Failed to fetch module %s: %d") % ToJs(specifier) % ec;
  return ec;
}

static JsErrorCode
MyNotifyModuleReadyCallback(_In_opt_ JsModuleRecord referencingModule, _In_opt_ JsValueRef exceptionVar) {
  if (exceptionVar) {
    LOGE("%s") % JsToString(exceptionVar);
    return JsNoError;
  }
  JsValueRef result;
  return JsModuleEvaluation(referencingModule, &result);
}

static JsErrorCode NativeInitializeImportMetaCallback(JsModuleRecord referencingModule, JsValueRef importMetaVar) try {
  if (!importMetaVar) return JsNoError;
  JsValueRef defined, url;
  ThrowError(JsGetModuleHostInfo(referencingModule, JsModuleHostInfo_HostDefined, &defined));
  ThrowError(JsGetModuleHostInfo(referencingModule, JsModuleHostInfo_Url, &url));
  JsObjectWrapper wrap{importMetaVar};
  if (GetJsType(defined) == JsObject) {
    // Native module
    wrap["native"] = defined;
    ThrowError(JsSetModuleHostInfo(referencingModule, JsModuleHostInfo_HostDefined, url));
  }
  wrap["url"] = url;
  return JsNoError;
} catch (JsErrorCode ec) { return ec; }

// static JsErrorCode
// NotifyNativeModuleReadyCallback(_In_opt_ JsModuleRecord referencingModule, _In_opt_ JsValueRef exceptionVar) try {
//   JsValueRef result, nameref;
//   ThrowError(JsModuleEvaluation(referencingModule, &result));
//   JsGetModuleHostInfo(referencingModule, JsModuleHostInfo_HostDefined, &nameref);
//   auto ns   = JsObjectWrapper::FromModuleRecord(referencingModule);
//   auto name = FromJs<std::string>(nameref);
//   LOGV("native");
//   ModuleRegister::GetList()[name](ns);
//   return JsNoError;
// } catch (JsErrorCode ec) { return ec; }