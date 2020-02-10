#pragma once

#include <functional>
#include <vector>
#include "sig.h"

namespace Mod {

template <std::uint32_t sig, typename... Params> class EventEmitter {
public:
  using CallBackType = std::function<void(Params...)>;

private:
  std::vector<CallBackType> _vector;

public:
  void AddListener(sigt<sig>, CallBackType fn) { _vector.emplace_back(fn); }
  void RemoveListener(sigt<sig>, CallBackType fn) {
    auto it = _vector.find(fn);
    if (it != _vector.end()) { _vector.erase(it); }
  }

protected:
  void Emit(sigt<sig>, Params... params) {
    for (auto &fn : _vector) { fn(std::forward<Params>(params)...); }
  }
};

} // namespace Mod

#define USING_EVENTEMITTER(sig, ...)                                                                                   \
private:                                                                                                               \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::Emit;                                                             \
                                                                                                                       \
public:                                                                                                                \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::AddListener;                                                      \
  using ::Mod::EventEmitter<sig##_sig, __VA_ARGS__>::RemoveListener