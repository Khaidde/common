#include "common/util/mutex.hpp"

void Mutex::init() {
  ticket  = 0;
  serving = 0;
}

void Mutex::lock() {
  u32 my_ticket = __sync_fetch_and_add(&ticket, 1);
  while (my_ticket != serving) {
    // spinlock
  }
}

void Mutex::unlock() { ++serving; }
