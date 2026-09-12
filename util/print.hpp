#ifndef COMMON_UTIL_PRINT_HPP
#define COMMON_UTIL_PRINT_HPP

#include "common/general.hpp"
#include "common/util/mutex.hpp"

extern Mutex console_output_mutex;

#if DEBUG
bool check_debug_tag(cstr message);
#endif

template <typename... Args>
void dbg(cstr message, Args... args) {
#if DEBUG
  if (!check_debug_tag(message)) return;

  console_output_mutex.lock();
  printf("debug: ");
  printf(message, args...);
  console_output_mutex.unlock();
#else
  return;
  printf(message, args...); // Dummy use of "args".
#endif
}

void error(cstr message, ...);

[[noreturn]] void panic(cstr message, ...);

#endif
