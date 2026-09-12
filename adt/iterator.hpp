#ifndef COMMON_ADT_ITERATOR_HPP
#define COMMON_ADT_ITERATOR_HPP

#include "common/general.hpp"

template <typename T>
struct IteratorRange {
  T begin() { return begin_it; };

  T end() { return end_it; }

  bool at_least(i32 n) {
    assert(n > 0);
    auto curr_it = begin_it;
    for (i32 i = 0; i < n; ++i) {
      if (!(curr_it != end_it)) return false;
      ++curr_it;
    }
    return true;
  }

  bool at_least_one() { return begin_it && begin_it != end_it; }

  T begin_it;
  T end_it;
};

template <typename T>
struct ListIterator {
  T operator*() const { return *current; }

  T operator->() const { return *current; }

  ListIterator &operator++() {
    ++current;
    return *this;
  }

  ListIterator &operator--() {
    --current;
    return *this;
  }

  friend bool operator!=(const ListIterator &iterator1, const ListIterator &iterator2) {
    return iterator1.current != iterator2.current;
  }

  static IteratorRange<ListIterator<T>> range(T *data, i32 length) {
    IteratorRange<ListIterator<T>> res;
    res.begin_it = {data};
    res.end_it   = {data + length};
    return res;
  }

  T *current;
};

template <typename T>
struct PointerListIterator {
  T *operator*() const { return current; }

  T *operator->() const { return current; }

  PointerListIterator &operator++() {
    ++current;
    return *this;
  }

  friend bool operator!=(const PointerListIterator &iterator1, const PointerListIterator &iterator2) {
    return iterator1.current != iterator2.current;
  }

  static IteratorRange<PointerListIterator<T>> range(T *data, i32 length) {
    IteratorRange<PointerListIterator<T>> res;
    res.begin_it = {data};
    res.end_it   = {data + length};
    return res;
  }

  T *current;
};

template <typename T>
struct ReverseListIterator {
  T operator*() const { return *current; }

  T operator->() const { return *current; }

  ReverseListIterator &operator++() {
    --current;
    return *this;
  }

  ReverseListIterator &operator--() {
    ++current;
    return *this;
  }

  friend bool operator!=(const ReverseListIterator &iterator1, const ReverseListIterator &iterator2) {
    return iterator1.current != iterator2.current;
  }

  static IteratorRange<ReverseListIterator<T>> range(T *data, i32 length) {
    IteratorRange<ReverseListIterator<T>> res;
    res.begin_it = {data + length - 1};
    res.end_it   = {data - 1};
    return res;
  }

  T *current;
};

#endif
