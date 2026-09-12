#ifndef COMMON_ADT_HASH_HPP
#define COMMON_ADT_HASH_HPP

#include "common/general.hpp"

namespace impl {

using HashValue = u64;

template <typename K>
struct HashFn {
  static_assert(!sizeof(K), "No hash function implemented for type");
  HashValue operator()(K /*unused*/) { return 0; }
};

template <typename K>
struct HashFn<K *> {
  HashValue operator()(K *pointer) { return uintptr_t(pointer) >> ct_log2<sizeof(K)>::value; }
};

template <>
struct HashFn<i32> {
  HashValue operator()(i32 number) { return u64(number); }
};

template <>
struct HashFn<cstr> {
  HashValue operator()(cstr string) {
    HashValue hash = 1;
    while (*string) {
      hash = ((hash << 5U) - hash) + HashValue(*string);
      ++string;
    }
    return hash;
  }
};

template <typename K>
struct EqualFn {
  static_assert(!sizeof(K), "No equal function implemented for type");
  bool operator()(K /*unused*/, K /*unused*/) { return false; }
};

template <typename K>
struct EqualFn<K *> {
  bool operator()(K *pointer1, K *pointer2) { return pointer1 == pointer2; }
};

template <>
struct EqualFn<i32> {
  bool operator()(i32 number1, i32 number2) { return number1 == number2; }
};

template <>
struct EqualFn<cstr> {
  bool operator()(cstr string1, cstr string2) {
#if DEBUG
    while (*string1 && *string2) {
      if (*string1 != *string2) return false;
      ++string1;
      ++string2;
    }
    return *string1 == *string2;
#else
    return strcmp(string1, string2);
#endif
  }
};

} // namespace impl

#endif
