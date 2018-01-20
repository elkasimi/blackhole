#pragma once

#include <stdint.h>

struct Settings {
  int32_t max_iterations;
  double max_time;
  const struct Position* position;
};

struct Result {
  int32_t move;
  char log[1024];
  double delta_time;
};

struct Result mcts_best_move(const struct Settings* settings);
