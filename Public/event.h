#pragma once

#include <functional>
#include <vector>
#include <boost/core/noncopyable.hpp>
#include "sig.h"

namespace Mod {

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
  // Remove event listener, the first parameter is used to distinguish different event overloads (SIG("eventname"))
  void RemoveListener(sigt<sig>, CallBackType fn) {
    auto it = std::find(_vector.begin(), _vector.end(), fn);
    if (it != _vector.end()) { _vector.erase(it); }
  }

protected:
  // Emit event, the first parameter is used to distinguish different event overloads (SIG("eventname"))
  void Emit(sigt<sig>, Params... params) {
    for (auto &fn : _vector) fn(std::forward<Params>(params)...);
  }
};

} // namespace Mod

// Be sure to use this macro to make multiple inherited function overloading effective
#define USING_EVENTEMITTER(sig, ...)                                                                                   \
private:                                                                                                               \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::Emit;                                                             \
                                                                                                                       \
public:                                                                                                                \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::AddListener;                                                      \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::RemoveListener