#pragma once

#include <cstdio>
#include <optional>
#include <functional>
#include <stdexcept>
#include <string>
#include <cstdint>

#include <ChakraCore.h>

#define ThrowError(fnret)                                                                                              \
  if (JsErrorCode ec = fnret; ec != JsNoError) throw ec;
#define ReturnError(fnret)                                                                                             \
  if (JsErrorCode ec = fnret; ec != JsNoError) return ec;

namespace Mod::Scripting {

inline JsValueRef operator""_js(char const *str, size_t length) {
  JsValueRef ref;
  ThrowError(JsCreateString(str, length, &ref));
  return ref;
} // namespace Mod::ScriptinginlineJsValueRefoperator""_js(charconst*str,size_tlength)

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

inline JsValueRef ToJs(std::string const &str) {
  JsValueRef ref;
  ThrowError(JsCreateString(str.data(), str.size(), &ref));
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

template <typename T> inline JsValueRef ToJs(std::optional<T> opt) {
  if (opt) return ToJs(*opt);
  return GetUndefined();
}

inline JsPropertyIdRef ToJsP(char const *str) {
  JsPropertyIdRef ref;
  ThrowError(JsCreatePropertyId(str, strlen(str), &ref));
  return ref;
}

inline JsPropertyIdRef ToJsP(std::string_view str) {
  JsPropertyIdRef ref;
  ThrowError(JsCreatePropertyId(str.data(), str.length(), &ref));
  return ref;
}

template <typename T, typename F> inline JsValueRef ToJsArray(T t, F f) {
  JsValueRef ref;
  ThrowError(JsCreateArray(t.size(), &ref));
  int idx = 0;
  for (auto it : t) ThrowError(JsSetIndexedProperty(ref, ToJs(idx++), f(it)));
  return ref;
}

template <typename T> inline JsValueRef ToJsArray(T t) {
  JsValueRef ref;
  ThrowError(JsCreateArray(t.size(), &ref));
  int idx = 0;
  for (auto it : t) ThrowError(JsSetIndexedProperty(ref, ToJs(idx++), ToJs(it)));
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

inline std::string JsToString(JsValueRef ref) {
  JsValueRef str;
  ThrowError(JsConvertValueToString(ref, &str));
  return FromJs<std::string>(str);
}

struct Arguments {
  JsValueRef self;
  JsValueRef newTarget;
  bool isConstructCall;
  JsValueRef *arguments;
  unsigned short argumentCount;

  Arguments(JsValueRef *arguments, unsigned short argumentCount, JsNativeFunctionInfo *info)
      : arguments(arguments), argumentCount(argumentCount) {
    if (info) {
      self            = info->thisArg;
      newTarget       = info->newTargetArg;
      isConstructCall = info->isConstructCall;
    } else {
      self = *arguments;
    }
  }

  constexpr size_t size() noexcept { return argumentCount - 1; }
  constexpr auto begin() noexcept { return arguments + 1; }
  constexpr auto end() noexcept { return arguments + argumentCount; }
  constexpr bool empty() noexcept { return argumentCount <= 1; }
  constexpr JsValueRef operator[](unsigned short idx) {
    if (idx >= argumentCount - 1) throw std::out_of_range{"index"};
    return arguments[idx + 1];
  }
};

struct JsConvertible {
  JsValueRef ref;

  struct Raw {
    JsValueRef ref;
  };

  using FnType = std::function<JsValueRef(JsValueRef callee, Arguments)>;

  JsConvertible(JsNativeFunction fn, void *state = nullptr) { ThrowError(JsCreateFunction(fn, state, &ref)); }

  JsConvertible(JsNativeFunction fn, char const *name, void *state = nullptr) {
    ThrowError(JsCreateNamedFunction(ToJs(name), fn, state, &ref));
  }

  JsConvertible(JsEnhancedNativeFunction fn, JsConvertible val, void *state = nullptr) {
    ThrowError(JsCreateEnhancedFunction(fn, val, state, &ref));
  }

  JsConvertible(FnType fn, char const *name = nullptr) {
    auto nfn = new FnType(std::move(fn));
    ThrowError(JsCreateEnhancedFunction(
        [](JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, JsNativeFunctionInfo *info,
           void *state) -> JsValueRef {
          FnType &rfn = *(FnType *) state;
          try {
            return rfn(callee, {arguments, argumentCount, info});
          } catch (std::exception const &ex) {
            JsSetException(ToJs(ex.what()));
            return GetUndefined();
          }
        },
        name ? ToJs(name) : JS_INVALID_REFERENCE, nfn, &ref));
  }

  template <typename T, typename C, typename... PS> JsConvertible(T (C::*fn)(PS...), char const *name = nullptr) {
    union {
      T (C::*fn)(PS...);
      void *state;
    } u;
    u.fn = fn;
    ThrowError(JsCreateEnhancedFunction(
        [](JsValueRef callee, JsValueRef *arguments, unsigned short argumentCount, JsNativeFunctionInfo *info,
           void *state) -> JsValueRef {
          try {
            union {
              T (C::*fn)(PS...);
              void *state;
            } u;
            u.state           = state;
            T (C::*fn)(PS...) = u.fn;
            Arguments args{arguments, argumentCount, info};
            if (args.size() != sizeof...(PS))
              throw std::runtime_error{"Require " + std::to_string(sizeof...(PS)) + " argument"};
            JsObjectWarpper xself{args.self};
            auto lfn = [&]<std::size_t... I>(std::index_sequence<I...> seq) {
              using tp = std::tuple<PS...>;
              return (xself.GetExternalData<C>()->*fn)(FromJs<std::tuple_element_t<I, tp>>(args[I])...);
            };
            return ToJs(lfn(std::make_index_sequence<sizeof...(PS)>{}));
          } catch (std::exception const &ex) {
            JsSetException(ToJs(ex.what()));
            return GetUndefined();
          }
        },
        name ? ToJs(name) : JS_INVALID_REFERENCE, u.state, &ref));
  }

  operator JsValueRef() { return ref; };
};

struct PropertyDesc;

struct JsObjectWarpper {
  JsValueRef ref;

  struct PropertyDesc {
    std::function<JsValueRef(JsObjectWarpper)> get;
    std::function<void(JsObjectWarpper, JsValueRef)> set;

    PropertyDesc(std::function<JsValueRef(JsObjectWarpper)> get, std::function<void(JsObjectWarpper, JsValueRef)> set)
        : get(get), set(set) {}

    template <typename T> PropertyDesc(JsValueRef (T::*tget)(), void (T::*tset)(JsValueRef) = nullptr) {
      if (tget) get = [=](JsObjectWarpper obj) -> JsValueRef { return (obj.GetExternalData<T>()->*tget)(); };
      if (tset) set = [=](JsObjectWarpper obj, JsValueRef rhs) { (obj.GetExternalData<T>()->*tset)(rhs); };
    }
    template <typename T, typename R> PropertyDesc(R (T::*tget)(), void (T::*tset)(R) = nullptr) {
      if (tget) get = [=](JsObjectWarpper obj) -> JsValueRef { return ToJs((obj.GetExternalData<T>()->*tget)()); };
      if (tset) set = [=](JsObjectWarpper obj, JsValueRef rhs) { (obj.GetExternalData<T>()->*tset)(FromJs<R>(rhs)); };
    }

    JsValueRef toObject() {
      JsObjectWarpper ret;
      ret["configurable"] = false;
      ret["enumerable"]   = true;
      if (get)
        ret["get"] = [get = this->get](JsValueRef callee, Arguments args) -> JsValueRef {
          JsObjectWarpper self{args.self};
          return get(self);
        };
      if (set)
        ret["set"] = [set = this->set](JsValueRef callee, Arguments args) -> JsValueRef {
          JsObjectWarpper self{args.self};
          set(self, args[0]);
          return GetUndefined();
        };
      return *ret;
    }
  };

  struct PropProxy {
    JsValueRef ref;
    JsPropertyIdRef name;

    JsValueType type() { return GetJsType(fetch()); }

    void set(JsValueRef rhs) {
      ThrowError(JsSetProperty(ref, name, rhs, true));
      temp = nullptr;
    }

    bool define(PropertyDesc desc) {
      bool result;
      ThrowError(JsDefineProperty(ref, name, desc.toObject(), &result));
      temp = nullptr;
      return result;
    }

    template <typename T> JsValueRef operator=(T val) {
      if constexpr (std::is_same_v<T, JsValueRef>) {
        set(val);
        return val;
      } else if constexpr (std::is_same_v<T, JsObjectWarpper> || std::is_same_v<T, JsConvertible>) {
        set(val.ref);
        return val.ref;
      } else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, double>) {
        auto o = ToJs(val);
        set(o);
        return o;
      } else if constexpr (std::is_same_v<T, PropertyDesc>) {
        define(val);
        return nullptr;
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
      ThrowError(JsConvertValueToString(fetch(), &str));
      return FromJs<std::string>(str);
    }

  private:
    JsValueRef temp;

    PropProxy(JsValueRef ref, JsPropertyIdRef name) : ref(ref), name(name) {}

    friend struct JsObjectWarpper;

    JsValueRef fetch() {
      if (!temp) JsGetProperty(ref, name, &temp);
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
    JsValueRef ref = nullptr;
    ThrowError(JsGetAndClearExceptionWithMetadata(&ref));
    return JsObjectWarpper{ref};
  }
  static JsObjectWarpper FromModuleRecord(JsModuleRecord mod) {
    JsValueRef ref = nullptr;
    ThrowError(JsGetModuleNamespace(mod, &ref));
    return JsObjectWarpper{ref};
  }
  template <typename T> static JsObjectWarpper FromExternalObject(T *type, JsValueRef prototype = nullptr) {
    JsValueRef ref          = nullptr;
    JsFinalizeCallback fini = [](void *ptr) { delete (T *) ptr; };
    if (prototype) {
      ThrowError(JsCreateExternalObjectWithPrototype(type, fini, prototype, &ref));
    } else {
      ThrowError(JsCreateExternalObject(type, fini, &ref));
    }
    return JsObjectWarpper{ref};
  }

  template <typename T> T *GetExternalData() {
    void *temp;
    ThrowError(JsGetExternalData(ref, &temp));
    return (T *) temp;
  }

  PropProxy operator[](char const *name) const { return {ref, ToJsP(name)}; }

  // bool DefineProperty(JsPropertyIdRef id, PropertyDesc desc) {
  //   bool result;
  //   ThrowError(JsDefineProperty(ref, id, desc.toObject(), &result));
  //   return result;
  // }

  JsValueRef operator*() const { return ref; }
};
} // namespace Mod::Scripting