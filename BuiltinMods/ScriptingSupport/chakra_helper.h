#pragma once

#include <cstdio>
#include <functional>
#include <stdexcept>
#include <string>

#include "ChakraCommon.h"
#include <cstdint>

#include <ChakraCore.h>
#include <vcruntime.h>

#define ThrowError(fnret)                                                                                              \
  if (JsErrorCode ec = fnret; ec != JsNoError) throw ec;
#define ReturnError(fnret)                                                                                             \
  if (JsErrorCode ec = fnret; ec != JsNoError) return ec;

inline JsValueRef operator""_js(char const *str, size_t length) {
  JsValueRef ref;
  ThrowError(JsCreateString(str, length, &ref));
  return ref;
}

inline JsValueRef operator""_js(const wchar_t *str, size_t length) {
  JsValueRef ref;
  ThrowError(JsCreateStringUtf16((const uint16_t *) str, length, &ref));
  return ref;
}

inline JsPropertyIdRef operator""_jsp(char const *str, size_t length) {
  JsPropertyIdRef prop;
  ThrowError(JsCreatePropertyId(str, length, &prop));
  return prop;
}

inline JsValueRef operator""_jsp(const wchar_t *str, size_t length) {
  JsValueRef ref;
  ThrowError(JsCreateStringUtf16((const uint16_t *) str, length, &ref));
  return ref;
}

inline JsValueRef ToJs(char const *str) {
  JsValueRef ref;
  ThrowError(JsCreateString(str, strlen(str), &ref));
  return ref;
}

inline JsValueRef ToJs(bool flag) {
  JsValueRef ref;
  ThrowError(JsBoolToBoolean(flag, &ref));
  return ref;
}

inline JsValueRef ToJs(int val) {
  JsValueRef ref;
  ThrowError(JsIntToNumber(val, &ref));
  return ref;
}

inline JsValueRef ToJs(double val) {
  JsValueRef ref;
  ThrowError(JsDoubleToNumber(val, &ref));
  return ref;
}

inline JsValueRef ToJs(JsNativeFunction fn) {
  JsValueRef ref;
  ThrowError(JsCreateFunction(fn, nullptr, &ref));
  return ref;
}

template <typename Type> Type FromJs(JsValueRef ref);

template <> inline bool FromJs(JsValueRef ref) {
  bool val;
  ThrowError(JsBooleanToBool(ref, &val));
  return val;
}

template <> inline std::string FromJs(JsValueRef ref) {
  std::string val;
  int length;
  size_t _;
  ThrowError(JsGetStringLength(ref, &length));
  val.resize(length);
  ThrowError(JsCopyString(ref, val.data(), length, &_));
  return val;
}

template <> inline std::wstring FromJs(JsValueRef ref) {
  wchar_t const *buffer;
  size_t length;
  ThrowError(JsStringToPointer(ref, &buffer, &length));
  return std::wstring(buffer, length);
}

template <> inline int FromJs(JsValueRef ref) {
  int val;
  ThrowError(JsNumberToInt(ref, &val));
  return val;
}

template <> inline double FromJs(JsValueRef ref) {
  double val;
  ThrowError(JsNumberToDouble(ref, &val));
  return val;
}

inline JsValueType GetJsType(JsValueRef ref) {
  JsValueType out;
  ThrowError(JsGetValueType(ref, &out));
  return out;
}

inline JsValueRef GetUndefined() {
  JsValueRef ret;
  JsGetUndefinedValue(&ret);
  return ret;
}

struct JsConvertible {
  JsValueRef ref;

  struct Raw {
    JsValueRef ref;
  };

  struct Arguments {
    JsValueRef *arguments;
    unsigned short argumentCount;

    constexpr auto self() noexcept { return arguments; }
    constexpr size_t size() noexcept { return argumentCount - 1; }
    constexpr auto begin() noexcept { return arguments + 1; }
    constexpr auto end() noexcept { return arguments + argumentCount; }
    constexpr bool empty() noexcept { return argumentCount <= 1; }
    constexpr JsValueRef operator[](unsigned short idx) {
      if (idx >= argumentCount - 1) throw std::out_of_range{"index"};
      return arguments[idx - 1];
    }
  };

  using FnType         = std::function<JsValueRef(JsValueRef callee, bool isConstructCall, Arguments)>;
  using EnhancedFnType = std::function<JsValueRef(JsValueRef callee, Arguments, JsNativeFunctionInfo *info)>;

  JsConvertible(Raw raw) : ref(raw.ref) {}
  JsConvertible(int val) : ref(ToJs(val)) {}
  JsConvertible(double val) : ref(ToJs(val)) {}
  JsConvertible(char const *val) : ref(ToJs(val)) {}

  JsConvertible(JsNativeFunction fn, void *state = nullptr) { ThrowError(JsCreateFunction(fn, state, &ref)); }

  JsConvertible(JsNativeFunction fn, char const *name, void *state = nullptr) {
    ThrowError(JsCreateNamedFunction(ToJs(name), fn, state, &ref));
  }

  JsConvertible(JsEnhancedNativeFunction fn, JsConvertible val, void *state = nullptr) {
    ThrowError(JsCreateEnhancedFunction(fn, val, state, &ref));
  }

  JsConvertible(FnType fn) {
    auto nfn = new FnType(std::move(fn));
    ThrowError(JsCreateFunction(
        [](JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount,
           void *state) -> JsValueRef {
          FnType &rfn = *(FnType *) state;
          return rfn(callee, isConstructCall, {arguments, argumentCount});
        },
        nfn, &ref));
  }

  JsConvertible(FnType fn, char const *name) {
    auto nfn = new FnType(std::move(fn));
    ThrowError(JsCreateNamedFunction(
        ToJs(name),
        [](JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount,
           void *state) -> JsValueRef {
          FnType &rfn = *(FnType *) state;
          return rfn(callee, isConstructCall, {arguments, argumentCount});
        },
        nfn, &ref));
  }

  JsConvertible(EnhancedFnType fn, JsConvertible val) {
    auto nfn = new EnhancedFnType(std::move(fn));
    ThrowError(JsCreateEnhancedFunction(
        [](JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, JsNativeFunctionInfo *info,
           void *state) -> JsValueRef {
          EnhancedFnType &rfn = *(EnhancedFnType *) state;
          return rfn(callee, {arguments, argumentCount}, info);
        },
        val, nfn, &ref));
  }

  operator JsValueRef() { return ref; };
};

struct JsObjectWarpper {
  JsValueRef ref;

  struct PropProxy {
    JsValueRef ref, name;

    JsValueType type() { return GetJsType(fetch()); }

    void set(JsValueRef rhs) {
      ThrowError(JsObjectSetProperty(ref, name, rhs, true));
      temp = nullptr;
    }

    template <typename T> JsValueRef operator=(T val) {
      if constexpr (std::is_same_v<T, JsValueType>) {
        set(val);
        return val;
      } else if constexpr (std::is_same_v<T, JsObjectWarpper> || std::is_same_v<T, JsConvertible>) {
        set(val.ref);
        return val.ref;
      } else {
        JsConvertible o{val};
        set(o.ref);
        return o;
      }
    }

    JsValueRef operator*() { return fetch(); }

    template <typename T> T get() {
      if constexpr (std::is_same_v<T, JsValueType>) {
        return ref;
      } else if constexpr (std::is_same_v<T, JsObjectWarpper>) {
        return JsObjectWarpper{ref};
      } else {
        return FromJs<T>(ref);
      }
    }

    std::string ToString() {
      JsValueRef str;
      ThrowError(JsConvertValueToString(ref, &str));
      return FromJs<std::string>(str);
    }

  private:
    JsValueRef temp;

    PropProxy(JsValueRef ref, JsValueRef name) : ref(ref), name(name) {}

    friend struct JsObjectWarpper;

    JsValueRef fetch() {
      if (!temp) JsObjectGetProperty(ref, name, &temp);
      return temp;
    }
  };

  explicit JsObjectWarpper(JsValueRef ref) : ref(ref) {}
  explicit JsObjectWarpper() { ThrowError(JsCreateObject(&ref)); }

  static JsObjectWarpper FromGlobal() {
    JsValueRef ref;
    ThrowError(JsGetGlobalObject(&ref));
    return JsObjectWarpper{ref};
  }
  static JsObjectWarpper FromCurrentException() {
    JsValueRef ref;
    ThrowError(JsGetAndClearExceptionWithMetadata(&ref));
    return JsObjectWarpper{ref};
  }

  PropProxy operator[](char const *name) { return {ref, ToJs(name)}; }

  operator JsValueRef() { return ref; }
};