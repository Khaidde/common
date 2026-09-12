#include "common/mem.hpp"

#if DEBUG
GlobalMemoryStats global_memory_statistics;

GlobalMemoryStats::~GlobalMemoryStats() {
  dbg("[memory] Bytes MMAP'd:  %8db\n", global_memory_statistics.bytes_mmap);
  dbg("[memory] Bytes Requested: %8db\n", global_memory_statistics.bytes_requested);
  dbg("[memory] Bytes Used:      %8db\n", global_memory_statistics.bytes_used);
  dbg("[memory] Bytes Freelist:  %8db\n", global_memory_statistics.bytes_freelist);
}
#endif
