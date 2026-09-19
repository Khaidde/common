#ifndef COMMON_MEM_HPP
#define COMMON_MEM_HPP

#include <sys/mman.h>

#include <cstring>

#include "common/general.hpp"
#include "common/util/print.hpp"

#if DEBUG
extern struct GlobalMemoryStats {
  ~GlobalMemoryStats();

  i32 bytes_mmap      = 0;
  i32 bytes_requested = 0;
  i32 bytes_used      = 0;
  i32 bytes_freelist  = 0;
} global_memory_statistics;
#endif

static constexpr i32 align(i32 bytes) { return (bytes + i32(sizeof(intptr_t)) - 1) & ~(i32(sizeof(intptr_t)) - 1); }

template <typename T>
void memory_copy(T *destination, const T *source, i32 count) {
#if DEBUG
  for (i32 i = 0; i < count; ++i) destination[i] = source[i];
#else
  memcpy(destination, source, usize(count) * sizeof(T));
#endif
}

template <typename T>
void memory_set(T *destination, u8 value, i32 count) {
#if DEBUG
  for (i32 i = 0; i < i32(sizeof(T)) * count; ++i) *(((u8 *)destination) + i) = value;
#else
  memset(destination, value, usize(count) * sizeof(T));
#endif
}

template <typename T>
bool memory_equal(const T *src1, const T *src2, i32 count) {
#if DEBUG
  for (i32 i = 0; i < i32(sizeof(T)) * count; ++i) {
    if (*(((u8 *)src1) + i) != *(((u8 *)src2) + i)) return false;
  }
  return true;
#else
  return memcmp(src1, src2, usize(count) * sizeof(T)) == 0;
#endif
}

template <typename T>
void memory_swap(T *first, T *second, i32 count) {
  T temp;
  memory_copy(&temp, first, count);
  memory_copy(first, second, count);
  memory_copy(second, &temp, count);
}

template <typename T>
T *memory_calloc(i32 count) {
  return (T *)malloc(sizeof(T) * u32(count));
}

struct ArenaAllocator {
  static const i32 capacity = 64 * 1024 * 1024;

  void init() {
    offset = 0;
    data   = (i8 *)mmap(nullptr, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#if DEBUG
    global_memory_statistics.bytes_mmap += capacity;
#endif
  }

  void destroy() {
    assert(data);
    munmap(data, capacity);
  }

  template <typename T>
  T *construct(size count = 1) {
    // Ensure all allocations are aligned to the size of a pointer.
    i32 aligned_bytes = align(i32(sizeof(T)) * count);
#if DEBUG
    global_memory_statistics.bytes_requested += size(sizeof(T)) * count;
    global_memory_statistics.bytes_used += aligned_bytes;
    if (offset + aligned_bytes > capacity) panic("[arena_allocator] Exceeded capacity of %d bytes.\n", capacity);

    memory_set(data + offset, 0xFE, usize(aligned_bytes));
#endif
    auto *pointer = (T *)(data + offset);
    offset += aligned_bytes;
    return pointer;
  }

  void reset_offset(i32 saved_offset) {
    assert(saved_offset >= 0 && saved_offset <= offset);
#if DEBUG
    global_memory_statistics.bytes_used -= (offset - saved_offset);
#endif
    offset = saved_offset;
  }

  i8 *data;
  i32 offset;
};

template <typename T, i32 chunks_per_block = 8>
struct FreelistAllocator {
  struct Chunk {
    i32 metadata;
    Chunk *next;
  };

  static const i32 chunk_size = align(sizeof(T));

  void init() {
    head = nullptr;
#if DEBUG
    object_count = 0;
#endif
  }

  T *construct(ArenaAllocator *allocator) {
    if (!head) {
      head = (Chunk *)allocator->construct<char>(chunks_per_block * chunk_size);
#if DEBUG
      global_memory_statistics.bytes_freelist += chunks_per_block * chunk_size;
#endif

      Chunk *curr = head;
      for (i32 i = 0; i < chunks_per_block - 1; ++i) {
        curr->next = (Chunk *)((i8 *)curr + chunk_size);
        curr       = curr->next;
      }
      curr->next = nullptr;
    }

    Chunk *chunk = head;
    head         = head->next;
#if DEBUG
    memory_set((i8 *)chunk, 0xFE, chunk_size);
    dbg("[freelist] %p: alloc n=%d: %p\n", (void *)this, ++object_count, (void *)chunk);
#endif
    return (T *)chunk;
  }

  void destruct(T *pointer) {
#if DEBUG
    assert(object_count > 0);
    dbg("[freelist] %p: free n=%d: %p\n", (void *)this, --object_count, (void *)pointer);
#endif
    memory_set((i8 *)pointer, 0xBA, chunk_size);
    ((Chunk *)pointer)->next = head;
    head                     = (Chunk *)pointer;
  }

  Chunk *head;
#if DEBUG
  i32 object_count;
#endif
};

#endif
