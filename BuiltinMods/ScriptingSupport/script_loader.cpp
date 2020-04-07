#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>

#include <ChakraCore.h>

#include <log.h>
#include <string>

#include "ChakraCommon.h"
#include "chakra_helper.h"
#include "global.h"
#include "apiset.h"

namespace fs = std::filesystem;

DEF_LOGGER("ScriptLoader");

static std::map<fs::path, JsModuleRecord> mod_cache;

static JsErrorCode FetchImportedModule(
    _In_ JsModuleRecord referencingModule, _In_ JsValueRef specifier,
    _Outptr_result_maybenull_ JsModuleRecord *dependentModuleRecord);

static JsErrorCode FetchImportedModuleFromScript(
    _In_ JsSourceContext dwReferencingSourceContext, _In_ JsValueRef specifier,
    _Outptr_result_maybenull_ JsModuleRecord *dependentModuleRecord);

static JsErrorCode
MyNotifyModuleReadyCallback(_In_opt_ JsModuleRecord referencingModule, _In_opt_ JsValueRef exceptionVar);

// static JsErrorCode copyString(JsValueRef jsv, std::string &ws);

static void LoadModuleFromFile(JsModuleRecord module, fs::path path, fs::path full) {
  static JsSourceContext ctx = 0;
  auto cookies               = ctx++;
  JsValueRef exception;

  JsValueRef real;
  auto str = path.native();
  ThrowError(JsCreateStringUtf16((uint16_t const *) str.c_str(), str.size(), &real));
  ThrowError(JsSetModuleHostInfo(module, JsModuleHostInfo_HostDefined, real));
  ThrowError(JsSetModuleHostInfo(module, JsModuleHostInfo_Url, real));

  std::ifstream ifs{full};
  if (!ifs) throw JsErrorInvalidArgument;
  std::string content{std::istreambuf_iterator{ifs}, {}};

  auto xec = JsParseModuleSource(
      module, cookies, (BYTE *) content.data(), content.size(), JsParseModuleSourceFlags_DataIsUTF8, &exception);
  if (exception) JsSetException(exception);
  ThrowError(xec);
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
  // Fake while to allow break out
  try {
    auto metadata  = JsObjectWarpper::FromCurrentException();
    auto exception = metadata["exception"].ToString();
    auto line      = metadata["line"].get<int>();
    auto column    = metadata["column"].get<int>();
    auto url       = metadata["url"].get<std::string>();
    LOGE("Exception %s from %s[%d:%d]") % exception % url % line % column;
  } catch (...) {}
  LOGE("Failed to load root module: %s") % hasException;
} catch (JsErrorCode ec) { LOGE("Failed to load: %d") % ec; }

static JsErrorCode copyString(JsValueRef jsv, std::string &ws) {
  char buffer[2048];
  size_t length;
  auto ec = JsCopyString(jsv, buffer, sizeof buffer, &length);
  if (ec != JsNoError) return ec;
  ws = std::string(buffer, length);
  return ec;
}

static JsErrorCode resolveModule(JsValueRef specifier, fs::path &path, fs::path &full) {
  std::wstring addition = FromJs<std::wstring>(specifier);
  JsErrorCode ec        = JsNoError;

  path /= addition;
  path.make_preferred();
  path = fs::weakly_canonical(path);
  full = "scripts" / path;
  return ec;
}

static JsErrorCode
FetchModule(JsModuleRecord referencingModule, JsValueRef specifier, JsModuleRecord *dependentModuleRecord) {
  fs::path base;
  fs::path full;

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
    *dependentModuleRecord = it->second;
    return JsNoError;
  }

  ThrowError(JsInitializeModuleRecord(referencingModule, specifier, dependentModuleRecord));
  LoadModuleFromFile(*dependentModuleRecord, base, full);

  mod_cache[base] = *dependentModuleRecord;

  return JsNoError;
}

static JsErrorCode FetchImportedModule(
    _In_ JsModuleRecord referencingModule, _In_ JsValueRef specifier,
    _Outptr_result_maybenull_ JsModuleRecord *dependentModuleRecord) {
  return FetchModule(referencingModule, specifier, dependentModuleRecord);
}

static JsErrorCode FetchImportedModuleFromScript(
    _In_ JsSourceContext dwReferencingSourceContext, _In_ JsValueRef specifier,
    _Outptr_result_maybenull_ JsModuleRecord *dependentModuleRecord) {
  return FetchModule(nullptr, specifier, dependentModuleRecord);
}

static JsErrorCode
MyNotifyModuleReadyCallback(_In_opt_ JsModuleRecord referencingModule, _In_opt_ JsValueRef exceptionVar) {
  JsValueRef result;
  return JsModuleEvaluation(referencingModule, &result);
}