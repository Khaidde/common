#ifndef COMMON_ADT_STRING_HPP
#define COMMON_ADT_STRING_HPP

#include "common/adt/hash.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

struct StringRef {
  i32 length;
  cstr data;

  inline cstr to_cstr(ArenaAllocator *allocator) {
    auto *buffer = allocator->construct<char>(length + 1);
    for (i32 i = 0; i < length; ++i) buffer[i] = data[i];
    buffer[length] = '\0';
    return buffer;
  }

  void dump(FILE *out) {
    for (i32 i = 0; i < length; ++i) putc(data[i], out);
  }
};

inline StringRef strref(cstr string) {
  StringRef result;
  result.data   = string;
  result.length = 0;
  while (*string) {
    ++result.length;
    ++string;
  }
  return result;
}

inline bool strref_equal(StringRef string1, StringRef string2) {
  if (string1.length != string2.length) return false;
  return memory_equal(string1.data, string2.data, string1.length);
}

namespace impl {

// Modified from rapidhash V3 by Nicolas De Carli.

inline void rapid_mum(u64 *A, u64 *B) {
  u128 r = *A;
  r *= *B;
  *A = u64(r);
  *B = u64(r >> 64);
}

inline u64 rapid_mix(u64 A, u64 B) {
  rapid_mum(&A, &B);
  return A ^ B;
}

inline u64 rapidhash_nano(cstr key, i32 len) {
  static constexpr u64 secret0 = 0x2d358dccaa6c78a5ull;
  static constexpr u64 secret1 = 0x8bb84b93962eacc9ull;
  static constexpr u64 secret2 = 0x4b33a62ed433d4a3ull;
  static constexpr u64 secret3 = 0xaaaaaaaaaaaaaaaaull;

  assert(len > 0);
  const u8 *p = (const u8 *)key;
  u64 seed    = rapid_mix(secret2, secret1);
  u64 a       = 0;
  u64 b       = 0;
  usize i     = usize(len);
  if (len <= 16) {
    if (len >= 4) {
      seed ^= usize(len);
      if (len >= 8) {
        const u8 *plast = p + len - 8;
        a               = *(u64 *)p;
        b               = *(u64 *)plast;
      } else {
        const u8 *plast = p + len - 4;
        a               = *(u32 *)p;
        b               = *(u32 *)plast;
      }
    } else if (len > 0) {
      a = (((u64)p[0]) << 45) | p[len - 1];
      b = p[len >> 1];
    }
  } else {
    if (i > 48) {
      u64 see1 = seed, see2 = seed;
      do {
        seed = rapid_mix(*(u64 *)p ^ secret0, *(u64 *)(p + 8) ^ seed);
        see1 = rapid_mix(*(u64 *)(p + 16) ^ secret1, *(u64 *)(p + 24) ^ see1);
        see2 = rapid_mix(*(u64 *)(p + 32) ^ secret2, *(u64 *)(p + 40) ^ see2);
        p += 48;
        i -= 48;
      } while (i > 48);
      seed ^= see1;
      seed ^= see2;
    }
    if (i > 16) {
      seed = rapid_mix(*(u64 *)p ^ secret2, *(u64 *)(p + 8) ^ seed);
      if (i > 32) seed = rapid_mix(*(u64 *)(p + 16) ^ secret2, *(u64 *)(p + 24) ^ seed);
    }
    a = *(u64 *)(p + i - 16) ^ i;
    b = *(u64 *)(p + i - 8);
  }
  a ^= secret1;
  b ^= seed;
  rapid_mum(&a, &b);
  return rapid_mix(a ^ secret3, b ^ secret1 ^ i);
}

template <>
struct HashFn<StringRef> {
  HashValue operator()(StringRef string) { return HashValue(rapidhash_nano(string.data, string.length)); }
};

template <>
struct EqualFn<StringRef> {
  bool operator()(StringRef string1, StringRef string2) { return strref_equal(string1, string2); }
};

} // namespace impl

#endif
