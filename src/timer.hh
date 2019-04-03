#ifndef TIMER_HH
#define TIMER_HH

#include <sys/time.h>

struct timestamp {
  struct timeval t;

  timestamp operator-(timestamp t2) const {
    timestamp ret;
    timersub(&t, &t2.t, &ret.t);
    return ret;
  }

  // Duration functions
  int64_t millis() const { return t.tv_sec * 1000 + t.tv_usec/1000; }
  int64_t secs() const { return t.tv_sec; }
};

inline timestamp now() {
  timestamp t;
  gettimeofday(&t.t, nullptr);
  return t;
}

#endif
