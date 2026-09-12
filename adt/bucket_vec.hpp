#ifndef COMMON_ADT_BUCKET_VEC_HPP
#define COMMON_ADT_BUCKET_VEC_HPP

#include "common/adt/iterator.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

// Avoids reallocation of existing elements upon dynamic resize.
// Two-levels of indirection on random access.
// Useful when we want persistent pointers.
// Also useful when T has a large size and list is large.
template <typename T>
struct BucketVec {
  static const i32 log2_bucket_size      = 4;
  static const i32 bucket_size_minus_one = 15;
  static_assert(1 << log2_bucket_size == bucket_size_minus_one + 1);

  struct Iterator {
    T operator*() const { return parent->get(index); }

    Iterator &operator++() {
      ++index;
      return *this;
    }

    Iterator &operator--() {
      --index;
      return *this;
    }

    friend bool operator!=(const Iterator &it1, const Iterator &it2) {
      assert(it1.parent == it2.parent);
      return it1.index != it2.index;
    }

    i32 index;
    BucketVec<T> *parent;
  };

  struct ReversePointerIterator {
    T *operator*() const { return parent->ref(index); }

    ReversePointerIterator &operator++() {
      --index;
      return *this;
    }

    ReversePointerIterator &operator--() {
      ++index;
      return *this;
    }

    friend bool operator!=(const ReversePointerIterator &it1, const ReversePointerIterator &it2) {
      assert(it1.parent == it2.parent);
      return it1.index != it2.index;
    }

    i32 index;
    BucketVec<T> *parent;
  };

  struct Bucket {
    T data[bucket_size_minus_one + 1];
  };

  void init() {
    bucket_table   = nullptr;
    bucket_count   = 0;
    table_capacity = 0;
    length         = 0;
  }

  Iterator begin() { return {0, this}; }

  Iterator end() { return {length, this}; }

  IteratorRange<ReversePointerIterator> reverse_pointer_iterable() {
    IteratorRange<ReversePointerIterator> res;
    res.begin_it = {length - 1, this};
    res.end_it   = {-1, this};
    return res;
  }

  void reserve(ArenaAllocator *allocator, i32 new_item_count) {
    assert(new_item_count >= 0);
    while (bucket_count << log2_bucket_size < new_item_count) {
      if (bucket_count + 1 > table_capacity) {
        table_capacity   = (table_capacity << 1) + 4;
        auto *newBuckets = allocator->construct<Bucket *>(table_capacity);
        if (bucket_table) {
          memory_copy(newBuckets, bucket_table, bucket_count);
        }
        bucket_table = newBuckets;
      }
      bucket_table[bucket_count++] = allocator->construct<Bucket>(1);
    }
  }

  T front() {
    assert(length > 0);
    return bucket_table[0]->data[0];
  }

  T *front_ref() {
    assert(length > 0);
    return &bucket_table[0]->data[0];
  }

  T back() {
    assert(length > 0);
    return bucket_table[(length - 1) >> log2_bucket_size]->data[(length - 1) & bucket_size_minus_one];
  }

  T *back_ref() {
    assert(length > 0);
    return &bucket_table[(length - 1) >> log2_bucket_size]->data[(length - 1) & bucket_size_minus_one];
  }

  T *push_empty(ArenaAllocator *allocator) {
    reserve(allocator, ++length);
    return back_ref();
  }

  void push(ArenaAllocator *allocator, T &value) {
    reserve(allocator, length + 1);
    bucket_table[length >> log2_bucket_size]->data[length & bucket_size_minus_one] = value;
    ++length;
  }

  void push(ArenaAllocator *allocator, T &&value) { push(allocator, value); }

  T get(i32 index) {
    assert(index < length);
    return bucket_table[index >> log2_bucket_size]->data[index & bucket_size_minus_one];
  }

  T *ref(i32 index) {
    assert(index < length);
    return &bucket_table[index >> log2_bucket_size]->data[index & bucket_size_minus_one];
  }

  Bucket **bucket_table;
  i32 bucket_count;
  i32 table_capacity;
  i32 length;
};

#endif
