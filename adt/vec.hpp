#ifndef COMMON_ADT_VEC_HPP
#define COMMON_ADT_VEC_HPP

#include "common/adt/iterator.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

template <typename T>
struct Vec {
  void init() {
    data     = nullptr;
    length   = 0;
    capacity = 0;
  }

  ListIterator<T> begin() { return ListIterator<T>::range(data, length).begin(); }

  ListIterator<T> end() { return ListIterator<T>::range(data, length).end(); }

  IteratorRange<PointerListIterator<T>> ptr_iterable() { return PointerListIterator<T>::range(data, length); }

  IteratorRange<ReverseListIterator<T>> reverse_iterable() { return ReverseListIterator<T>::range(data, length); }

  void reserve(ArenaAllocator *allocator, i32 new_capacity) {
    assert(new_capacity >= 0);
    if (capacity < new_capacity) {
      do {
        // cap = cap * 1.5 + 8.
        capacity = (capacity << 1) - (capacity >> 1) + 8;
      } while (capacity < new_capacity);
      resize(allocator, capacity);
    }
  }

  void resize(ArenaAllocator *allocator, i32 new_capacity) {
    T *new_data = allocator->construct<T>(new_capacity);
    if (data) memory_copy(new_data, data, length);
    data     = new_data;
    capacity = new_capacity;
  }

  void clear() { length = 0; }

  void push(ArenaAllocator *allocator, T &value) {
    reserve(allocator, length + 1);
    data[length++] = value;
  }

  void push(ArenaAllocator *allocator, T &&value) { push(allocator, value); }

  T *push_empty(ArenaAllocator *allocator, i32 count) {
    reserve(allocator, length + count);
    length += count;
    return back_ref();
  }

  void push_at_index(ArenaAllocator *allocator, T &value, i32 index) {
    assert(0 <= index && index <= length);
    reserve(allocator, ++length);
    for (i32 i = length - 1; i > index; --i) data[i] = data[i - 1];
    data[index] = value;
  }

  void push_at_index(ArenaAllocator *allocator, T &&value, i32 index) { push_at_index(allocator, value, index); }

  void remove(i32 index) {
    assert(0 <= index && index < length);
    if (index < length - 1) data[index] = back();
    pop();
  }

  void pop() {
    assert(length > 0);
    --length;
  }

  T get(i32 index) {
    assert(0 <= index && index < length);
    return data[index];
  }

  T *ref(i32 index) {
    assert(0 <= index && index < length);
    return &data[index];
  }

  T front() {
    assert(length > 0);
    return data[0];
  }

  T *front_ref() {
    assert(length > 0);
    return &data[0];
  }

  T back() {
    assert(length > 0);
    return data[length - 1];
  }

  T *back_ref() {
    assert(length > 0);
    return &data[length - 1];
  }

  T *data;
  i32 length;
  i32 capacity;
};

template <typename T>
struct VecView {
  void init(Vec<T> *vec, i32 start, i32 view_length) {
    data   = vec->get_reference(start);
    length = view_length;
  }

  ListIterator<T> begin() { return {data}; }

  ListIterator<T> end() { return {&data[length]}; }

  T *data;
  i32 length;
};

#endif
