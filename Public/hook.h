#pragma once

#include <Windows.h>
#include <exception>
#include <iostream>
#include <cstdio>
#include "sig.h"

#ifdef ModLoader_EXPORTS
#  define HOOKAPI __declspec(dllexport)
#else
#  define HOOKAPI __declspec(dllimport)
#endif

extern "C" {
// The core api of the hook function
HOOKAPI int HookFunction(void *oldfunc, void **poutold, void *newfunc);
// Used to get a server-defined specific function by name
HOOKAPI void *GetServerSymbol(char const *name);
}

// Used to get a server-defined specific function by name
template <typename T> T *GetServerSymbol(char const *name) {
  union {
    T *target;
    void *source;
  } u;
  u.source = GetServerSymbol(name);
  return u.target;
}

// Used to get a server-defined specific function by name
template <typename T> T GetServerFunctionSymbol(char const *name) {
  union {
    T target;
    void *source;
  } u;
  u.source = GetServerSymbol(name);
  return u.target;
}

// A convenience function for setting a virtual function table
template <typename T> inline void SetVirtualTable(T *self, char const *name) {
  union {
    T *self;
    void **pvtable;
  } u;
  u.self     = self;
  *u.pvtable = GetServerSymbol(name);
}

// A convenience function for calling a specific server-defined function
template <typename Ret, typename... Params> inline auto CallServerFunction(char const *name, Params... params) -> Ret {
  auto fn = GetServerFunctionSymbol<Ret (*)(Params...)>(name);
  return fn(params...);
}

// A convenience function for calling a specific server-defined function (for instance method)
template <typename Ret, typename Class, typename... Params>
inline auto CallServerClassMethod(char const *name, Class const *self, Params... params) -> Ret {
  auto fn = GetServerFunctionSymbol<Ret (Class::*)(Params...)>(name);
  return (((Class *) self)->*fn)(params...);
}

class SymbolNotFound : std::exception {
  char buffer[4096 + 30];

  inline char const *fillBuffer(char const *s) {
    sprintf_s(buffer, "Failed to found symbol %s", s);
    return buffer;
  }

public:
  inline SymbolNotFound(char const *s) : exception(fillBuffer(s)) {}
};

class FailedToHook : std::exception {
public:
  HOOKAPI FailedToHook(int code);
};

#ifndef ModLoader_EXPORTS

class THookRegister {
public:
  THookRegister(void *address, void *hook, void **org) {
    auto ret = HookFunction(address, org, hook);
    if (ret != 0) {
      std::cerr << "FailedToHook: " << address << std::endl;
      throw FailedToHook(ret);
    }
  }
  THookRegister(char const *sym, void *hook, void **org) {
    auto found = GetServerSymbol(sym);
    if (found == nullptr) {
      std::cerr << "SymbolNotFound: " << sym << std::endl;
      throw SymbolNotFound(sym);
    }
    auto ret = HookFunction(found, org, hook);
    if (ret != 0) {
      std::cerr << "FailedToHook: " << sym << std::endl;
      throw FailedToHook(ret);
    }
  }
  template <typename T> THookRegister(const char *sym, T hook, void **org) {
    union {
      T a;
      void *b;
    } hookUnion;
    hookUnion.a = hook;
    THookRegister(sym, hookUnion.b, org);
  }
  template <typename T> THookRegister(void *sym, T hook, void **org) {
    union {
      T a;
      void *b;
    } hookUnion;
    hookUnion.a = hook;
    THookRegister(sym, hookUnion.b, org);
  }
};

#  define VA_EXPAND(...) __VA_ARGS__

template <std::uint32_t sig> struct THookTemplate;
template <std::uint32_t sig> extern THookRegister THookRegisterTemplate;

#  define _TInstanceHook(class_inh, pclass, iname, sym, ret, ...)                                                      \
    template <> struct THookTemplate<iname##_sig> class_inh {                                                          \
      typedef ret (THookTemplate::*original_type)(__VA_ARGS__);                                                        \
      static original_type &_original() {                                                                              \
        static original_type storage;                                                                                  \
        return storage;                                                                                                \
      }                                                                                                                \
      template <typename... Params> static ret original(pclass *_this, Params &&... params) {                          \
        return (((THookTemplate *) _this)->*_original())(std::forward<Params>(params)...);                             \
      }                                                                                                                \
      ret _hook(__VA_ARGS__);                                                                                          \
    };                                                                                                                 \
    template <>                                                                                                        \
    static THookRegister THookRegisterTemplate<iname##_sig>{sym, &THookTemplate<iname##_sig>::_hook,                   \
                                                            (void **) &THookTemplate<iname##_sig>::_original()};       \
    ret THookTemplate<iname##_sig>::_hook(__VA_ARGS__)

#  define _TInstanceDefHook(iname, sym, ret, type, ...)                                                                \
    _TInstanceHook( : public type, type, iname, sym, ret, VA_EXPAND(__VA_ARGS__))
#  define _TInstanceNoDefHook(iname, sym, ret, ...) _TInstanceHook(, void, iname, sym, ret, VA_EXPAND(__VA_ARGS__))

#  define _TStaticHook(pclass, iname, sym, ret, ...)                                                                   \
    template <> struct THookTemplate<iname##_sig> pclass {                                                             \
      typedef ret (*original_type)(__VA_ARGS__);                                                                       \
      static original_type &_original() {                                                                              \
        static original_type storage;                                                                                  \
        return storage;                                                                                                \
      }                                                                                                                \
      template <typename... Params> static ret original(Params &&... params) {                                         \
        return _original()(std::forward<Params>(params)...);                                                           \
      }                                                                                                                \
      static ret _hook(__VA_ARGS__);                                                                                   \
    };                                                                                                                 \
    template <>                                                                                                        \
    static THookRegister THookRegisterTemplate<iname##_sig>{sym, &THookTemplate<iname##_sig>::_hook,                   \
                                                            (void **) &THookTemplate<iname##_sig>::_original()};       \
    ret THookTemplate<iname##_sig>::_hook(__VA_ARGS__)

#  define _TStaticDefHook(iname, sym, ret, type, ...)                                                                  \
    _TStaticHook( : public type, iname, sym, ret, VA_EXPAND(__VA_ARGS__))
#  define _TStaticNoDefHook(iname, sym, ret, ...) _TStaticHook(, iname, sym, ret, VA_EXPAND(__VA_ARGS__))

#  define THook2(iname, ret, sym, ...) _TStaticNoDefHook(iname, sym, ret, VA_EXPAND(__VA_ARGS__))
#  define THook(ret, sym, ...) THook2(sym, ret, sym, VA_EXPAND(__VA_ARGS__))
#  define TStaticHook2(iname, ret, sym, type, ...) _TStaticDefHook(iname, sym, ret, type, VA_EXPAND(__VA_ARGS__))
#  define TStaticHook(ret, sym, type, ...) TStaticHook2(sym, ret, sym, type, VA_EXPAND(__VA_ARGS__))
#  define TClasslessInstanceHook2(iname, ret, sym, ...) _TInstanceNoDefHook(iname, sym, ret, VA_EXPAND(__VA_ARGS__))
#  define TClasslessInstanceHook(ret, sym, ...) TClasslessInstanceHook2(sym, ret, sym, VA_EXPAND(__VA_ARGS__))
#  define TInstanceHook2(iname, ret, sym, type, ...) _TInstanceDefHook(iname, sym, ret, type, VA_EXPAND(__VA_ARGS__))
#  define TInstanceHook(ret, sym, type, ...) TInstanceHook2(sym, ret, sym, type, VA_EXPAND(__VA_ARGS__))

#endif