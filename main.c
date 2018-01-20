#include "MCTS.h"

#include "IO.h"
#include "Position.h"
#include "TreeNode.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define STRING_EQUAL(a, b) (strcmp(a, b) == 0)

static struct Settings find_best_settings(const struct Position* position,
                                          double total_time) {
  const double ratio = 1.0;
  const double max_turn_time = 0.8 * ratio;
  const double max_time = 5.0 * ratio;
  const double rush_time = 4.0 * ratio;
  const int32_t max_iterations = 100000;

  struct Settings best_settings;
  best_settings.max_iterations = max_iterations;
  if (total_time < rush_time) {
    best_settings.max_time = max_turn_time;
  } else {
    int32_t remaining_moves = (1 + MAX_TURNS - position->turns) / 2;
    best_settings.max_time = (max_time - total_time) / remaining_moves;
  }
  best_settings.position = position;

  return best_settings;
}

void read_browns(struct Position* position, FILE* in) {
  for (int32_t i = 0; i < BROWN_COUNT; ++i) {
    char field[20];
    assert(fscanf(in, "%s", field) == 1);
    position_make_field_brown(position, field);
  }
  position_init_tiles_count(position);
}

int analyze(const char* filename) {
  FILE* in = fopen(filename, "r");

  if (!in) {
    fprintf(stderr, "File not found '%s'\n", filename);
    return -1;
  }

  struct Position position = create_start_position();

  read_browns(&position, in);

  char s[1024];
  while (fscanf(in, "%s", s) == 1) {
    int32_t move = io_read_move(s);
    position_play_move(&position, move);
  }

  while (position.turns != MAX_TURNS) {
    struct Settings settings = find_best_settings(&position, 0.0);
    struct Result result = mcts_best_move(&settings);
    position_play_move(&position, result.move);
    io_display_move(result.move, stdout);
    fprintf(stdout, "%s dt=%.2f\n", result.log, result.delta_time);
  }

  printf("End game!\n");

  fclose(in);

  return 0;
}

int main_loop() {
  fprintf(stderr, "size TreeNode = %lu\n", sizeof(struct TreeNode));
  double total_time = 0.05;

  struct Position position = create_start_position();

  read_browns(&position, stdin);

  char line[1024];
  assert(scanf("%s", line) == 1);
  while (!STRING_EQUAL(line, "Quit")) {
    if (!STRING_EQUAL(line, "Start")) {
      int32_t opponent_move = io_read_move(line);
      position_play_move(&position, opponent_move);
      io_display_move(opponent_move, stderr);
    }

    if (position.turns == MAX_TURNS) {
      break;
    }

    struct Settings settings = find_best_settings(&position, total_time);
    fprflush(stderr, "max-time=%.3f\n", settings.max_time);
    struct Result result = mcts_best_move(&settings);
    total_time += result.delta_time;
    fprflush(stderr, "%s dt=%.2f tt=%.2f\n", result.log, result.delta_time,
             total_time);
    position_play_move(&position, result.move);
    io_display_move(result.move, stderr);
    io_display_move(result.move, stdout);
    assert(scanf("%s", line) == 1);
  }

  return 0;
}

int main(int argc, char* args[]) {
  if (argc > 1) {
    return analyze(args[1]);
  }

  return main_loop();
}
