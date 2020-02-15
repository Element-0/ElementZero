#pragma once

#include <cstdint>

namespace mce {

class UUID {
public:
  std::uint64_t a, b;

  constexpr inline bool operator==(UUID const &rhs) const noexcept { return a == rhs.a && b == rhs.b; }
};

} // namespace mce