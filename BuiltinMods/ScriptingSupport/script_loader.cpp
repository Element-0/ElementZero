#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>

#include <ChakraCore.h>

#include <log.h>

#include "global.h"


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

static JsErrorCode getString(JsValueRef jsv, std::wstring &ws);
static JsErrorCode copyString(JsValueRef jsv, std::string &ws);

static JsErrorCode LoadModuleFromFile(JsModuleRecord module, fs::path path, fs::path full) {
  static JsSourceContext ctx = 0;
  auto cookies               = ctx++;
  JsErrorCode ec             = JsNoError;
  JsValueRef exception;

  JsValueRef real;
  auto str = path.native();
  ec       = JsCreateStringUtf16((uint16_t const *) str.c_str(), str.size(), &real);
  if (ec != JsNoError) return ec;
  ec = JsSetModuleHostInfo(module, JsModuleHostInfo_HostDefined, real);
  if (ec != JsNoError) return ec;
  ec = JsSetModuleHostInfo(module, JsModuleHostInfo_Url, real);
  if (ec != JsNoError) return ec;

  std::ifstream ifs{full};
  if (!ifs) return JsErrorInvalidArgument;
  std::string content{std::istreambuf_iterator{ifs}, {}};

  ec = JsParseModuleSource(
      module, cookies, (BYTE *) content.data(), content.size(), JsParseModuleSourceFlags_DataIsUTF8, &exception);
  if (exception) JsSetException(exception);
  return ec;
}

static JsErrorCode initRootModule(std::string_view path, JsModuleRecord *requestModule) {
  JsValueRef specifier;
  JsErrorCode ec = JsNoError;

  ec = JsCreateString(path.data(), path.size(), &specifier);
  if (ec != JsNoError) return ec;
  ec = JsInitializeModuleRecord(nullptr, specifier, requestModule);
  if (ec != JsNoError) return ec;
  ec = JsSetModuleHostInfo(*requestModule, JsModuleHostInfo_FetchImportedModuleCallback, (void *) FetchImportedModule);
  if (ec != JsNoError) return ec;
  ec = JsSetModuleHostInfo(
      *requestModule, JsModuleHostInfo_FetchImportedModuleFromScriptCallback, (void *) FetchImportedModuleFromScript);
  if (ec != JsNoError) return ec;
  ec = JsSetModuleHostInfo(
      *requestModule, JsModuleHostInfo_NotifyModuleReadyCallback, (void *) MyNotifyModuleReadyCallback);
  if (ec != JsNoError) return ec;
  ec = JsSetModuleHostInfo(*requestModule, JsModuleHostInfo_Url, specifier);
  if (ec != JsNoError) return ec;
  ec = JsSetModuleHostInfo(*requestModule, JsModuleHostInfo_HostDefined, specifier);
  if (ec != JsNoError) return ec;
  return ec;
}

void loadCustomScript() {
  JsModuleRecord root;

  if (!fs::exists("scripts/index.js")) {
    LOGW("Failed to load scripts/index.js");
    return;
  }

  auto ec = initRootModule("index.js", &root);
  if (ec != JsNoError) {
    LOGE("Failed to init root module: %d") % ec;
    return;
  }
  ec = LoadModuleFromFile(root, "index.js", "scripts/index.js");
  if (ec != JsNoError) {
    bool hasException;
    if (JsHasException(&hasException) != JsNoError) return;
    // Fake while to allow break out
    while (hasException) {
      JsValueRef metadata;
      if (JsGetAndClearExceptionWithMetadata(&metadata) != JsNoError) break;
      JsPropertyIdRef prop_exception, prop_line, prop_column, prop_url;
      JsValueRef exception, exception_str, line, column, url;
      std::string exception_name, url_data;
      int line_code, column_code;
      if (JsGetPropertyIdFromName(L"exception", &prop_exception) != JsNoError) break;
      if (JsGetPropertyIdFromName(L"line", &prop_line) != JsNoError) break;
      if (JsGetPropertyIdFromName(L"column", &prop_column) != JsNoError) break;
      if (JsGetPropertyIdFromName(L"url", &prop_url) != JsNoError) break;
      if (JsGetProperty(metadata, prop_exception, &exception) != JsNoError) break;
      if (JsGetProperty(metadata, prop_line, &line) != JsNoError) break;
      if (JsGetProperty(metadata, prop_column, &column) != JsNoError) break;
      if (JsGetProperty(metadata, prop_url, &url) != JsNoError) break;
      if (JsConvertValueToString(exception, &exception_str) != JsNoError) break;
      if (copyString(exception_str, exception_name) != JsNoError) break;
      if (JsNumberToInt(line, &line_code) != JsNoError) break;
      if (JsNumberToInt(column, &column_code) != JsNoError) break;
      if (copyString(url, url_data) != JsNoError) break;
      LOGE("Exception %s from %s[%d:%d]") % exception_name % url_data % line_code % column_code;
      break;
    }
    LOGE("Failed to load root module: %s") % hasException;
    return;
  }
}

static JsErrorCode getString(JsValueRef jsv, std::wstring &ws) {
  wchar_t const *buffer;
  size_t length;
  auto ec = JsStringToPointer(jsv, &buffer, &length);
  if (ec != JsNoError) return ec;
  ws = std::wstring(buffer, length);
  return ec;
}

static JsErrorCode copyString(JsValueRef jsv, std::string &ws) {
  char buffer[2048];
  size_t length;
  auto ec = JsCopyString(jsv, buffer, sizeof buffer, &length);
  if (ec != JsNoError) return ec;
  ws = std::string(buffer, length);
  return ec;
}

static JsErrorCode resolveModule(JsValueRef specifier, fs::path &path, fs::path &full) {
  std::wstring addition;
  JsErrorCode ec = JsNoError;

  ec = getString(specifier, addition);
  if (ec != JsNoError) return ec;
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
  JsErrorCode ec = JsNoError;

  if (referencingModule) {
    JsValueRef baseref;
    ec = JsGetModuleHostInfo(referencingModule, JsModuleHostInfo_HostDefined, &baseref);
    if (ec != JsNoError) return ec;
    std::wstring buf;
    ec = getString(baseref, buf);
    if (ec != JsNoError) return ec;
    base = buf;
    base = base.parent_path();
  }
  ec = resolveModule(specifier, base, full);
  if (ec != JsNoError) return ec;

  LOGV("try to load module %s") % base;

  if (auto it = mod_cache.find(base); it != mod_cache.end()) {
    *dependentModuleRecord = it->second;
    return JsNoError;
  }

  ec = JsInitializeModuleRecord(referencingModule, specifier, dependentModuleRecord);
  if (ec != JsNoError) return ec;

  ec = LoadModuleFromFile(*dependentModuleRecord, base, full);
  if (ec != JsNoError) return ec;

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