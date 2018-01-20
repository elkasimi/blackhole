#include "Random.h"

#include <math.h>
#include <stdlib.h>

int32_t random_int(int32_t bound) {
  double r = rand() / (RAND_MAX + 1.0);
  return (int32_t)floor(r * bound);
}
