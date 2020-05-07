#pragma once

#include <cstdint>
#include <cstddef>

#ifdef __clang__

#  define SIGTEMPLATE auto const &sig

namespace detail {
template <char...> struct sigv {};
template <char... ch> inline sigv<ch...> sigvx;
} // namespace detail

template <SIGTEMPLATE> struct sigt {};

template <typename C, C... ch> constexpr detail::sigv<ch...> const &operator""_sig() { return detail::sigvx<ch...>; }

#  define SIG(name)                                                                                                    \
    sigt<name##_sig> {}

inline sigt<"test"_sig> _sigt;

#else

#  define SIGTEMPLATE std::uint32_t sig

namespace detail {
constexpr std::uint32_t fnv1a_32(char const *s, std::size_t count) {
  return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}
} // namespace detail

template <std::uint32_t sig> struct sigt {};

// Unique type generated from a string
constexpr std::uint32_t operator""_sig(char const *s, std::size_t count) { return detail::fnv1a_32(s, count); }

// Unique type value generated from a string
#  define SIG(name)                                                                                                    \
    sigt<name##_sig> {}

#endif