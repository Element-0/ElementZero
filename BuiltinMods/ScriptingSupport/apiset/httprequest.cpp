#include "ChakraCommon.h"
#include "ChakraCore.h"
#include <MsXml6.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <boost/algorithm/string/trim.hpp>

#include <comhelper.h>
#include <mods/ScriptSupport/chakra_helper.h>
#include <mods/ScriptSupport/scriptapi.h>
#include <base/log.h>
#include <Core/ServerInstance.h>

#pragma comment(lib, "msxml6")

using namespace Mod::Scripting;
using namespace ComHelper;

DEF_LOGGER("HttpRequest");

struct HttpResponse {
  DWORD status;
  std::wstring statusText;
  std::map<std::wstring, std::wstring> headers;

  std::uint32_t GetStatus() const { return status; }
  std::wstring GetStatusText() const { return statusText; }
  JsValueRef GetHeaders() const {
    JsObjectWrapper obj;
    for (auto &[k, v] : headers) { obj[k.c_str()] = ToJs(v); }
    return obj.ref;
  }

  static JsValueRef InitProto() {
    static ValueHolder proto = IIFE([] {
      JsObjectWrapper ResponseProto;
      ResponseProto["status"]     = JsObjectWrapper::PropertyDesc{&HttpResponse::GetStatus};
      ResponseProto["statusText"] = JsObjectWrapper::PropertyDesc{&HttpResponse::GetStatusText};
      ResponseProto["headers"]    = JsObjectWrapper::PropertyDesc{&HttpResponse::GetHeaders};
      return *ResponseProto;
    });
    return *proto;
  }

  inline static JsObjectWrapper
  Create(DWORD status, std::wstring &&statusText, std::map<std::wstring, std::wstring> &&headers) {
    auto ret        = new HttpResponse();
    ret->status     = status;
    ret->statusText = std::move(statusText);
    ret->headers    = std::move(headers);
    return JsObjectWrapper::FromExternalObject(ret, InitProto());
  }
};

struct SimpleBuffer {
  BYTE *data{};
  size_t length{}, capacity{};

  ~SimpleBuffer() {
    if (data) std::free(data);
  }

  BYTE *prepare(size_t newlen) {
    capacity = length + newlen;
    data     = (BYTE *) std::realloc(data, length + newlen);
    return data + length;
  }

  void eat(size_t newlen) { length += newlen; }

  BYTE *dump() {
    auto ret = data;
    data     = NULL;
    length = capacity = 0;
    return ret;
  }
};

namespace {

template <typename T> class NotifyToken {
protected:
  std::mutex mtx;
  std::condition_variable cv;
  std::optional<T> value;
  std::exception_ptr ex;

public:
  inline void reset() {
    std::lock_guard lock{mtx};
    value.reset();
    ex = nullptr;
  }
  inline T wait() {
    std::unique_lock lock{mtx};
    cv.wait(lock, [this] { return ex || value.has_value(); });
    if (ex) std::rethrow_exception(ex);
    return *value;
  }
  inline void notify(T &&rhs) {
    {
      std::lock_guard lock{mtx};
      value.emplace(std::move(rhs));
    }
    cv.notify_one();
  }
  inline void failed(std::exception_ptr ptr) {
    {
      std::lock_guard lock{mtx};
      ex = ptr;
    }
    cv.notify_one();
  }
};

class HttpRequestCallback : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IXMLHTTPRequest2Callback> {
public:
  std::shared_ptr<ValueHolder> resolve, reject, stage;
  SimpleBuffer buffer;

  STDMETHODIMP
  OnRedirect(__RPC__in_opt IXMLHTTPRequest2 *pXHR, __RPC__in_string const WCHAR *pwszRedirectUrl) {
    UNREFERENCED_PARAMETER(pXHR);
    UNREFERENCED_PARAMETER(pwszRedirectUrl);
    return S_OK;
  }

  STDMETHODIMP
  OnHeadersAvailable(__RPC__in_opt IXMLHTTPRequest2 *pXHR, DWORD dwStatus, __RPC__in_string const WCHAR *pwszStatus) {
    DWORD status            = dwStatus;
    std::wstring statusText = pwszStatus;

    CoMemWrapper<WCHAR *> header_string;
    pXHR->GetAllResponseHeaders(&header_string);
    std::map<std::wstring, std::wstring> headers;
    if (header_string) {
      std::wistringstream iss{header_string};
      std::wstring key, value;
      while (std::getline(iss, key, L':') && std::getline(iss, value) && key.size()) {
        boost::trim(value);
        headers[key] = value;
      }
    }

    JsValueRef _promise, _resolve, _reject;

    NotifyToken<bool> token;

    LocateService<ServerInstance>()->queueForServerThread([&]() {
      stage =
          std::make_shared<ValueHolder>(HttpResponse::Create(status, std::move(statusText), std::move(headers)).ref);
      JsObjectWrapper obj{stage->ref};
      ThrowError(JsCreatePromise(&_promise, &_resolve, &_reject));
      obj["data"]       = _promise;
      JsValueRef arg[2] = {GetUndefined(), stage->ref};
      JsCallFunction(**resolve, arg, 2, NULL);
      token.notify(true);
    });

    token.wait();

    resolve->ref = _resolve;
    reject->ref  = _reject;
    return S_OK;
  }

  HRESULT fillBuffer(ISequentialStream *pStream) {
    constexpr auto MAX_BUFFER_LENGTH = 4096;
    HRESULT hr                       = S_OK;
    DWORD cbRead                     = 0;

    if (pStream == NULL) {
      hr = E_INVALIDARG;
      goto Exit;
    }

    for (;;) {
      hr = pStream->Read(buffer.prepare(MAX_BUFFER_LENGTH), MAX_BUFFER_LENGTH, &cbRead);

      if (FAILED(hr) || cbRead == 0) { goto Exit; }

      buffer.eat(cbRead);
    }

  Exit:

    return hr;
  }

  STDMETHODIMP
  OnDataAvailable(__RPC__in_opt IXMLHTTPRequest2 *pXHR, __RPC__in_opt ISequentialStream *pResponseStream) {
    fillBuffer(pResponseStream);
    return S_OK;
  }

  STDMETHODIMP
  OnResponseReceived(__RPC__in_opt IXMLHTTPRequest2 *pXHR, __RPC__in_opt ISequentialStream *pResponseStream) {
    fillBuffer(pResponseStream);

    LocateService<ServerInstance>()->queueForServerThread([=, len = buffer.length, data = buffer.dump()] {
      JsValueRef ret{};
      JsCreateExternalArrayBuffer(data, len, std::free, data, &ret);
      JsValueRef arg[2] = {GetUndefined(), ret};
      JsCallFunction(**resolve, arg, 2, NULL);
    });
    Release();
    pXHR->Release();
    return S_OK;
  }

  STDMETHODIMP
  OnError(__RPC__in_opt IXMLHTTPRequest2 *pXHR, HRESULT hrError) {
    LocateService<ServerInstance>()->queueForServerThread([=] {
      auto errstr = std::system_category().message(hrError);
      JsValueRef ret;
      JsCreateError(ToJs(errstr), &ret);
      JsValueRef arg[2] = {GetUndefined(), ret};
      JsCallFunction(**reject, arg, 2, NULL);
    });
    Release();
    pXHR->Release();
    return S_OK;
  }
};

} // namespace

static RegisterQueue queue("HttpRequest", [](JsObjectWrapper global) {
  global["HttpRequest"] = [](JsValueRef callee, Arguments args) -> JsValueRef {
    if (args.argumentCount == 2 || args.argumentCount == 3) {
      if (GetJsType(args[0]) != JsString || GetJsType(args[1]) != JsString) {
        throw std::runtime_error{"Require (string, string[, object])"};
      }
      auto method  = FromJs<std::wstring>(args[0]);
      auto methodc = FromJs<std::string>(args[0]);
      auto url     = FromJs<std::wstring>(args[1]);
      auto urlc    = FromJs<std::string>(args[1]);
      JsValueRef promise, resolve, reject;
      ThrowError(JsCreatePromise(&promise, &resolve, &reject));
      IXMLHTTPRequest2 *spXHR;
      HttpRequestCallback *spXhrCallback;

      HR{"create com object"} =
          CoCreateInstance(CLSID_FreeThreadedXMLHTTP60, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&spXHR));
      HR{"create callback object"} = MakeAndInitialize<HttpRequestCallback>(&spXhrCallback);

      spXhrCallback->resolve = std::make_shared<ValueHolder>(resolve);
      spXhrCallback->reject  = std::make_shared<ValueHolder>(reject);

      HR{"open http request"} = spXHR->Open(method.c_str(), url.c_str(), spXhrCallback, NULL, NULL, NULL, NULL);

      // if (args.argumentCount == 3) { return GetUndefined(); } // todo
      HR{"send http request"} = spXHR->Send(NULL, 0);

      LOGV("%s %s") % methodc % urlc;
      return promise;
    }
    return GetUndefined();
  };
});