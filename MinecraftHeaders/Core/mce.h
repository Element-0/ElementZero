#pragma once

#include <cstdint>
#include <string>

namespace mce {

class UUID {
public:
  std::uint64_t a, b;

  constexpr inline bool operator==(UUID const &rhs) const noexcept { return a == rhs.a && b == rhs.b; }

  inline operator char const *() const { return (char const *) this; }

  __declspec(dllimport) std::string asString() const;
  __declspec(dllimport) static UUID fromString(std::string const &source);
  __declspec(dllimport) static UUID EMPTY;

  inline bool empty() const noexcept { return a == 0 && b == 0; }
};

} // namespace mce