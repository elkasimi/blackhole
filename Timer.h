#pragma once

#include <sys/time.h>

struct timeval timer_now();

double timer_delta_time_since(const struct timeval* start);
