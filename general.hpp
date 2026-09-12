#ifndef COMMON_GENERAL_HPP
#define COMMON_GENERAL_HPP

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#if !defined(__clang__)
#error Compilation only supported with clang
#endif

#if !defined(__linux__)
#error Compilation only supported with linux
#endif

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error Big endian compilation not supported
#endif

#if defined(NDEBUG)
#define DEBUG 0
#else
#define DEBUG 1
#endif

#define CT_SIZEOF(x)                                                                                                   \
  static const int temp_tag = sizeof(x);                                                                               \
  template <int>                                                                                                       \
  struct Sizeof;                                                                                                       \
  static_assert(Sizeof<temp_tag>::temp_tag);

using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;
using u128  = __uint128_t;
using usize = size_t;

using i8   = int8_t;
using i16  = int16_t;
using i32  = int32_t;
using i64  = int64_t;
using size = ssize_t;

using cstr = const char *;

struct NullOptional {
  static const bool value = false;
};

extern const NullOptional nullopt;

template <typename T>
struct Optional {
  Optional(NullOptional /*unused*/) : has_value(false) {}
  Optional(T &val) : value(val), has_value(true) {}
  Optional(T &&val) : value(val), has_value(true) {}
  operator bool() { return has_value; }

  T &operator*() {
    assert(has_value);
    return value;
  }

  T *operator->() {
    assert(has_value);
    return &value;
  }

  T value;
  bool has_value;
};

template <i32 num>
struct ct_log2 {
  static const i32 value = 1 + ct_log2<num / 2>::value;
};

template <i32 num>
struct ct_is_pow2 {
  static const bool value = __builtin_popcount(num) == 1;
};

template <typename T>
T min(T a, T b) {
  return a < b ? a : b;
}

template <typename T>
T max(T a, T b) {
  return a > b ? a : b;
}

template <typename T>
i32 digit_count(T a) {
  i32 count = 0;
  while (a > 0) {
    ++count;
    a /= 10;
  }
  return count;
}

#endif
