#ifndef COMMON_ADT_MAP_HPP
#define COMMON_ADT_MAP_HPP

#include "common/adt/hash.hpp"
#include "common/adt/set.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

template <typename K, typename V>
struct Entry {
  K key;
  V value;
};

namespace impl {

template <typename K, typename V>
struct HashFn<Entry<K, V>> {
  HashValue operator()(Entry<K, V> entry) { return HashFn<K>()(entry.key); }
};

template <typename K, typename V>
struct EqualFn<Entry<K, V>> {
  bool operator()(Entry<K, V> entry1, Entry<K, V> entry2) { return EqualFn<K>()(entry1.key, entry2.key); }
};

} // namespace impl

template <typename K, typename V>
struct Map {
  using Key   = K;
  using Value = V;

  using EntryT = Entry<K, V>;

  using SetT = Set<EntryT>;

  struct InsertResult {
    bool inserted;
    V *value;
  };

  void init() { set.init(); }

  void clear() { set.clear(); }

  typename SetT::Iterator begin() { return set.begin(); }

  typename SetT::Iterator end() { return set.end(); }

  InsertResult insert_or_get(ArenaAllocator *allocator, Key &key, Value &value) {
    EntryT entry;
    entry.key   = key;
    entry.value = value;

    auto res = set.insert(allocator, entry);

    InsertResult insert_res;
    insert_res.inserted = res.inserted;
    insert_res.value    = &res.data->value;
    return insert_res;
  }

  InsertResult insert_or_get(ArenaAllocator *allocator, Key &&key, Value &val) {
    return insert_or_get(allocator, key, val);
  }

  InsertResult insert_or_get(ArenaAllocator *allocator, Key &key, Value &&val) {
    return insert_or_get(allocator, key, val);
  }

  InsertResult insert_or_get(ArenaAllocator *allocator, Key &&key, Value &&val) {
    return insert_or_get(allocator, key, val);
  }

  Value *ref(Key &key) {
    EntryT entry;
    entry.key  = key;
    auto *data = set.ref(entry);
    return data ? &data->value : nullptr;
  }

  Value *ref(Key &&key) { return ref(key); }

  SetT set;
};

#endif
