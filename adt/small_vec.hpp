#ifndef COMMON_ADT_SMALL_VEC_HPP
#define COMMON_ADT_SMALL_VEC_HPP

#include "common/adt/iterator.hpp"
#include "common/adt/vec.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

namespace impl {

template <typename T, typename B>
struct SmallVecSize {
  static const i32 preferred_size         = sizeof(T) > 8 ? 128 : 64;
  static const i32 preferred_num_elements = (preferred_size - sizeof(B)) / sizeof(T);
  static_assert(preferred_num_elements >= 1);
};

static_assert(impl::SmallVecSize<i32, Vec<i32>>::preferred_num_elements == 12);
static_assert(impl::SmallVecSize<i32 *, Vec<i32 *>>::preferred_num_elements == 6);

}; // namespace impl

template <typename T, i32 small_capacity = impl::SmallVecSize<T, Vec<T>>::preferred_num_elements>
struct SmallVec {
  void init() {
    base.init();
    base.data = (T *)&inline_storage;
  }

  ListIterator<T> begin() { return base.begin(); }

  ListIterator<T> end() { return base.end(); }

  IteratorRange<PointerListIterator<T>> ptr_iterable() { return base.ptr_iterable(); }

  IteratorRange<ReverseListIterator<T>> reverse_iterable() { return base.reverse_iterable(); }

  bool is_empty() { return base.length == 0; }

  void push(ArenaAllocator *allocator, T &value) {
    if (base.length == small_capacity) {
      base.reserve(allocator, base.length + 1);
      memory_copy(base.data, (T *)&inline_storage, base.length);
      base.data[base.length++] = value;
    } else if (base.data == inline_storage) {
      base.data[base.length++] = value;
    } else {
      base.push(allocator, value);
    }
  }

  void push(ArenaAllocator *allocator, T &&value) { push(allocator, value); }

  void pop() {
    assert(base.length > 0);
    --base.length;
  }

  T get(i32 index) {
    if (base.data == inline_storage) return inline_storage[index];
    return base.get(index);
  }

  T *ref(i32 index) {
    if (base.data == inline_storage) return &inline_storage[index];
    return base.ref(index);
  }

  T front() {
    assert(base.length > 0);
    return get(0);
  }

  T *front_ref() {
    assert(base.length > 0);
    return ref(0);
  }

  T back() {
    assert(base.length > 0);
    return get(base.length - 1);
  }

  T *back_ref() {
    assert(base.length > 0);
    return ref(base.length - 1);
  }

  i32 size() { return base.length; }

  T inline_storage[small_capacity];
  Vec<T> base;
};

#endif
