#pragma once

#include <cstdint>
#include <cstddef>

namespace detail {
constexpr std::uint32_t fnv1a_32(char const *s, std::size_t count) {
  return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}
} // namespace detail

template <std::uint32_t sig> struct sigt {};
constexpr std::uint32_t operator""_sig(char const *s, std::size_t count) { return detail::fnv1a_32(s, count); }

#define SIG(name)                                                                                                      \
  sigt<name##_sig> {}