#include "common/util/print.hpp"

#include <cstdarg>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <sys/resource.h>

Mutex console_output_mutex = {0, 0};

#if DEBUG
// String comparison of message in the format '[tag]' against the debug tag in the format 'tag1~tag2~tag3'.
bool check_debug_tag(cstr message) {
  // TODO: debug tag should be command line option. Currently hardcoded to 'sema' but should be ''.
  cstr tag = "sema";
  assert(message[0] == '[');

  cstr msg_ptr = message + 1;

  bool matches = true;
  while (*msg_ptr && *tag) {
    if (*tag == '~') {
      if (matches && *msg_ptr == ']') return true;
      matches = true;
      msg_ptr = message;
    } else if (*msg_ptr != *tag) {
      matches = false;
      while (*msg_ptr && *msg_ptr != ']') ++msg_ptr;
      while (*tag && *tag != '~') ++tag;
      continue;
    }
    ++msg_ptr;
    ++tag;
  }
  return matches && *msg_ptr == ']' && *tag == '\0';
}
#endif

void error(cstr message, ...) {
  fprintf(stderr, "error: ");
  va_list args;
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);
}

void print_backtrace() {
  static constexpr i32 max_frames = 100;
  i32 *buffer[max_frames];

  i32 nptrs = backtrace((void **)buffer, max_frames);
  for (i32 i = 0; i < nptrs; ++i) {
    Dl_info info;

    if (dladdr(buffer[i], &info) && info.dli_sname) {
      i32 status      = 0;
      char *demangled = abi::__cxa_demangle(info.dli_sname, NULL, NULL, &status);

      cstr name = (status == 0 && demangled) ? demangled : info.dli_sname;

      i32 offset = buffer[i] - (i32 *)info.dli_saddr;
      printf("%p : %s + 0x%x\n", (void *)buffer[i], name, offset);
    } else {
      printf("%p : [unknown]\n", (void *)buffer[i]);
    }
  }
}

void panic(cstr message, ...) {
#if DEBUG
  rlimit core_limit = {RLIM_INFINITY, RLIM_INFINITY};
  assert(setrlimit(RLIMIT_CORE, &core_limit) == 0);
  printf("Enabling core dumps...\n");
#endif
  fprintf(stderr, "PANIC: ");
  va_list args;
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);

  print_backtrace();

  abort();
}
