#ifndef COMMON_ADT_SET_HPP
#define COMMON_ADT_SET_HPP

#include "common/adt/hash.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

namespace impl {

// Robin hood algorithm based hashtable.
template <typename T, typename H, typename E>
struct Set {
  using Hash  = H;
  using Equal = E;

  using SetT = Set<T, H, E>;

  struct TableSlot {
    T data;
    i32 distance; // Distance from preferred hash slot, distance=0 means entry is empty.
  };

  struct InsertResult {
    bool inserted;
    T *data;
  };

  struct Iterator {
    T &operator*() const { return parent->table[index].data; }

    T *operator->() const { return &parent->table[index].data; }

    Iterator &operator++() {
      index = parent->get_valid_entry_index(index + 1);
      return *this;
    }

    friend bool operator==(const Iterator &iterator1, const Iterator &iterator2) {
      assert(iterator1.parent == iterator2.parent);
      return iterator1.index == iterator2.index;
    }

    friend bool operator!=(const Iterator &iterator1, const Iterator &iterator2) {
      assert(iterator1.parent == iterator2.parent);
      return iterator1.index != iterator2.index;
    }

    i32 index;
    SetT *parent;
  };

  void init() {
    capacity = 0;
    table    = nullptr;
    clear();
  }

  void clear() {
    length       = 0;
    max_distance = 0;
    memory_set(table, 0, capacity);
  }

  i32 get_valid_entry_index(i32 index) {
    while (index < capacity && !table[index].distance) index++;
    return index;
  }

  Iterator begin() { return {get_valid_entry_index(0), this}; }

  Iterator end() { return {capacity, this}; }

  InsertResult impl_insert(ArenaAllocator *allocator, T &temp_data) {
    InsertResult insert_res;
    // Load factor of 0.5.
    if (length + 1 > capacity >> 1) {
      auto *old_table  = table;
      i32 old_capacity = capacity;

      capacity = capacity ? capacity << 1 : 8;

      table = allocator->construct<TableSlot>(capacity);

      clear();

      for (i32 i = 0; i < old_capacity; ++i) {
        if (old_table[i].distance) insert(allocator, old_table[i].data);
      }
    }

    auto index   = Hash()(temp_data) & HashValue(capacity - 1);
    i32 distance = 1;
    for (i32 off = 0; off < capacity; ++off) {
      if (table[index].distance == 0) {
        if (distance > max_distance) max_distance = distance;

        table[index].data     = temp_data;
        table[index].distance = distance;
        ++length;

        insert_res.inserted = true;
        insert_res.data     = &table[index].data;
        return insert_res;
      }

      if (Equal()(table[index].data, temp_data)) {
        insert_res.inserted = false;
        insert_res.data     = &table[index].data;
        return insert_res;
      }

      if (table[index].distance < distance) {
        if (distance > max_distance) max_distance = distance;

        auto temp         = table[index].data;
        table[index].data = temp_data;
        temp_data         = temp;

        memory_swap(&table[index].distance, &distance, 1);
      }
      index = (index + 1) & HashValue(capacity - 1);
      ++distance;
    }
    assert(!"Hash table is unexpectedly full.\n");
  }

  InsertResult insert(ArenaAllocator *allocator, T data) { return impl_insert(allocator, data); }

  T *ref(T &data) {
    auto index = Hash()(data) & HashValue(capacity - 1);
    for (i32 off = 0; off < max_distance; ++off) {
      if (table[index].distance && Equal()(table[index].data, data)) return &table[index].data;
      index = (index + 1) & HashValue(capacity - 1);
    }
    return nullptr;
  }

  T *ref(T &&data) { return ref(data); }

  bool has(T &data) { return ref(data); }

  bool has(T &&data) { return ref(data); }

  TableSlot *table;
  i32 length;
  i32 capacity;
  i32 max_distance;
};

} // namespace impl

template <typename T>
using Set = impl::Set<T, impl::HashFn<T>, impl::EqualFn<T>>;

#endif
