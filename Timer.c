#include "Timer.h"

#include <stdlib.h>

struct timeval timer_now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv;
}

static double timeval_to_double(const struct timeval* tv) {
  return 1.0 * ((double)tv->tv_sec) + 1e-6 * ((double)tv->tv_usec);
}

double timer_delta_time_since(const struct timeval* start) {
  struct timeval current = timer_now();
  return timeval_to_double(&current) - timeval_to_double(start);
}
