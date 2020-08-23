#include "ChakraCommon.h"
#include "ChakraCore.h"
#include "modutils.h"
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
    static LeakedHolder proto = IIFE([] {
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

class StringReadStream : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ISequentialStream> {
public:
  std::string data;
  std::string_view buf;

  HRESULT STDMETHODCALLTYPE Read(
      /* [annotation] */
      _Out_writes_bytes_to_(cb, *pcbRead) void *pv,
      /* [annotation][in] */
      _In_ ULONG cb,
      /* [annotation] */
      _Out_opt_ ULONG *pcbRead) override {
    if (buf.empty()) {
      *pcbRead = 0;
      return S_OK;
    }
    auto toread = std::min((size_t) cb, buf.size());
    std::memcpy(pv, buf.data(), toread);
    buf.remove_prefix(toread);
    *pcbRead = (ULONG) toread;
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE Write(
      /* [annotation] */
      _In_reads_bytes_(cb) const void *pv,
      /* [annotation][in] */
      _In_ ULONG cb,
      /* [annotation] */
      _Out_opt_ ULONG *pcbWritten) override {
    return E_NOTIMPL;
  }
};

class HttpRequestCallback : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IXMLHTTPRequest2Callback> {
public:
  std::shared_ptr<ValueHolder> resolve, reject, stage;
  SimpleBuffer buffer;
  ISequentialStream *temp{};

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
      JsObjectWrapper obj{**stage};
      ThrowError(JsCreatePromise(&_promise, &_resolve, &_reject));
      *resolve          = _resolve;
      *reject           = _reject;
      obj["data"]       = _promise;
      JsValueRef arg[2] = {GetUndefined(), **stage};
      JsCallFunction(**resolve, arg, 2, NULL);
      token.notify(true);
    });

    token.wait();
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
    if (temp) temp->Release();
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
    if (temp) temp->Release();
    return S_OK;
  }
};

struct HttpOption {
  std::map<std::wstring, std::wstring> headers;
  StringReadStream *reqBody{};

  ~HttpOption() {
    if (reqBody) reqBody->Release();
  }

  void readFromJs(JsObjectWrapper obj) {
    auto oheaders = obj["headers"];
    auto obody    = obj["body"];
    if (oheaders.type() == JsObject) {
      JsObjectWrapper arr, xheaders{*oheaders};
      ThrowError(JsGetOwnPropertyNames(*oheaders, &arr));
      auto length = arr["length"].get<int>();
      LOGV("length %d") % length;
      for (int i = 0; i < length; i++) {
        auto key = arr[i].get<std::wstring>();
        LOGV("key %s") % arr[i].get<std::string>();
        auto value = xheaders[key.c_str()].get<std::wstring>();
        headers.emplace(std::move(key), std::move(value));
      }
    }
    if (obody.type() == JsArrayBuffer) {
      ChakraBytePtr buffer;
      unsigned int bufferLength;
      JsGetArrayBufferStorage(*obody, &buffer, &bufferLength);
      HR{"create body"} = MakeAndInitialize<StringReadStream>(&reqBody);
      reqBody->data     = {(char *) buffer, (size_t) bufferLength};
      reqBody->buf      = reqBody->data;
    } else if (obody.type() == JsTypedArray) {
      ChakraBytePtr buffer;
      unsigned int bufferLength;
      JsGetTypedArrayStorage(*obody, &buffer, &bufferLength, NULL, NULL);
      HR{"create body"} = MakeAndInitialize<StringReadStream>(&reqBody);
      reqBody->data     = {(char *) buffer, (size_t) bufferLength};
      reqBody->buf      = reqBody->data;
    }
  }
};

} // namespace

using pDllGetClassObject = HRESULT (*)(REFCLSID rclsid, REFIID riid, LPVOID ppv);

IXMLHTTPRequest2 *CreateXHR() {
  static auto DLL          = LoadLibrary(L"msxml6.dll");
  static auto GetClassObj  = (pDllGetClassObject) GetProcAddress(DLL, "DllGetClassObject");
  static auto ClassFactory = IIFE([] {
    IClassFactory *ret;
    HR{"create com factory"} = GetClassObj(CLSID_FreeThreadedXMLHTTP60, IID_IClassFactory, &ret);
    return ret;
  });
  IXMLHTTPRequest2 *ret;
  ClassFactory->CreateInstance(0, IID_IXMLHTTPRequest2, (void **) &ret);
  return ret;
}

static RegisterQueue queue("HttpRequest", [](JsObjectWrapper global) {
  global["HttpRequest"] = [](JsValueRef callee, Arguments args) -> JsValueRef {
    if (args.size() == 2 || args.size() == 3) {
      if (GetJsType(args[0]) != JsString || GetJsType(args[1]) != JsString) {
        throw std::runtime_error{"Require (string, string[, object])"};
      }
      auto method  = FromJs<std::wstring>(args[0]);
      auto methodc = FromJs<std::string>(args[0]);
      auto url     = FromJs<std::wstring>(args[1]);
      auto urlc    = FromJs<std::string>(args[1]);
      JsValueRef promise, resolve, reject;
      ThrowError(JsCreatePromise(&promise, &resolve, &reject));
      IXMLHTTPRequest2 *spXHR = CreateXHR();
      HttpRequestCallback *spXhrCallback;

      HttpOption opt;
      if (args.size() == 3 && GetJsType(args[2]) == JsObject) { opt.readFromJs(JsObjectWrapper{args[2]}); }

      HR{"create callback object"} = MakeAndInitialize<HttpRequestCallback>(&spXhrCallback);

      spXhrCallback->resolve = std::make_shared<ValueHolder>(resolve);
      spXhrCallback->reject  = std::make_shared<ValueHolder>(reject);

      HR{"open http request"} = spXHR->Open(method.c_str(), url.c_str(), spXhrCallback, NULL, NULL, NULL, NULL);

      for (auto &[k, v] : opt.headers) { spXHR->SetRequestHeader(k.c_str(), v.c_str()); }

      if (opt.reqBody) {
        HR{"send http request"} = spXHR->Send(opt.reqBody, (ULONG) opt.reqBody->data.size());
        opt.reqBody             = NULL;
      } else {
        HR{"send http request"} = spXHR->Send(NULL, 0);
      }

      LOGV("%s %s") % methodc % urlc;
      return promise;
    }
    throw std::runtime_error{"Require (string, string[, object])"};
  };
});