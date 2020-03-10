#pragma once

#include <functional>
#include <vector>
#include <set>
#include <optional>
#include <boost/core/noncopyable.hpp>
#include "sig.h"

namespace Mod {

template <typename T> class CallbackToken {
  std::optional<T> reason;

public:
  CallbackToken() {}
  operator T const &() const { return *reason; }
  operator std::optional<T>() const { return reason; }
  operator bool() const { return reason.has_value(); }
  T const &operator*() const { return *reason; }
  T const *operator->() const { return &*reason; }
  void operator()(T value) & { reason = {value}; }
};

// This class can be used to publish and subscribe to events
// Declaring multiple independent events through multiple inheritance
template <std::uint32_t sig, typename... Params> class EventEmitter : public boost::noncopyable {
public:
  using CallBackType = std::function<void(Params...)>;

private:
  std::vector<CallBackType> _vector;

public:
  // Add event listener, the first parameter is used to distinguish different event overloads (SIG("eventname"))
  void AddListener(sigt<sig>, CallBackType fn) { _vector.emplace_back(fn); }

protected:
  // Emit event, the first parameter is used to distinguish different event overloads (SIG("eventname"))
  void Emit(sigt<sig>, Params... params) {
    for (auto &fn : _vector) fn(std::forward<Params>(params)...);
  }
};

// This class can be used to publish and subscribe to events
// Declaring multiple independent events through multiple inheritance
template <std::uint32_t sig, typename... Params> class RecursiveEventEmitter : public boost::noncopyable {
public:
  class Trampoline;
  using CallBackType = std::function<void(Trampoline const &, Params...)>;

  struct Entry {
    CallBackType Callback;
    int Priority;

    Entry(CallBackType Callback) : Callback(Callback), Priority(1000) {}
    Entry(CallBackType Callback, int Priority) : Callback(Callback), Priority(Priority) {}

    constexpr bool operator<(Entry const &e) const noexcept { return Priority < e.Priority; }
    inline void operator()(Trampoline const &trampo, Params... params) const {
      Callback(trampo, std::forward<Params>(params)...);
    }
  };

private:
  std::set<Entry> _set;

public:
  class Trampoline {
    std::function<void(Trampoline const &, Entry const &)> f;
    typename std::set<Entry>::iterator it, end;

    Trampoline(
        std::function<void(Trampoline const &, Entry const &)> f, typename std::set<Entry>::iterator it,
        typename std::set<Entry>::iterator end)
        : f(f), it(it), end(end) {}
    friend class RecursiveEventEmitter;

  public:
    void operator()() const {
      if (it == end) return;
      auto cit = it;
      ++cit;
      f(Trampoline{f, cit, end}, *it);
    }
  };

public:
  // Add event listener, the first parameter is used to distinguish different event overloads (SIG("eventname"))
  void AddListener(sigt<sig>, Entry ent) { _set.emplace(ent); }

protected:
  // Emit event, the first parameter is used to distinguish different event overloads (SIG("eventname"))
  void Emit(sigt<sig>, Params... params) {
    Trampoline{[&](Trampoline const &tmp, Entry const &ent) { ent(tmp, std::forward<Params>(params)...); },
               _set.begin(), _set.end()}();
  }
};

template <typename Fn> auto RecursiveEventHandlerAdaptor(Fn fn) {
  return [=]<typename Tempo, typename... Params>(Tempo tempo, Params && ... params) {
    fn(std::forward<Params>(params)...);
    tempo();
  };
}

} // namespace Mod

// Be sure to use this macro to make multiple inherited function overloading effective
#define USING_EVENTEMITTER(sig, ...)                                                                                   \
private:                                                                                                               \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::Emit;                                                             \
                                                                                                                       \
public:                                                                                                                \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::AddListener;

// Be sure to use this macro to make multiple inherited function overloading effective
#define USING_RECEVENTEMITTER(sig, ...)                                                                                \
private:                                                                                                               \
  using ::Mod::RecursiveEventEmitter<sig##_sig, __VA_ARGS__>::Emit;                                                    \
                                                                                                                       \
public:                                                                                                                \
  using ::Mod::RecursiveEventEmitter<sig##_sig, __VA_ARGS__>::AddListener;