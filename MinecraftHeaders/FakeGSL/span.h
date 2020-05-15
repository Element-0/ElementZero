#pragma once

#include <cstdint>
#include <emmintrin.h>

namespace gsl {

template <typename T, size_t ext = (size_t) -1> class span;

template <typename T> class span<T, (size_t) -1> {
public:
  union {
    __m128i val;
    struct {
      T *a, *b;
    };
  };

  span(T *a, T *b) : a(a), b(b) {}

  T *begin() const { return a; }
  T *end() const { return b; }
  bool empty() const { return a == b; }
  size_t size() const { return b - a; }

  T *data() const { return a; }
};

template <typename T> span<T, (size_t) -1> make_span(T *target, size_t len) { return {target, target + len}; }

} // namespace gsl