#ifndef COMMON_UTIL_MUTEX_HPP
#define COMMON_UTIL_MUTEX_HPP

#include "common/general.hpp"

struct Mutex {
  void init();
  void lock();
  void unlock();

  volatile u16 ticket;
  volatile u16 serving;
};

#endif
