#pragma once

#include <stdbool.h>
#include <stdint.h>

#define MAX_VALUES 15
#define MAX_PLACES 36
#define BROWN_COUNT 5
#define MAX_TURNS 30

#define INVALID_MOVE 0
#define CREATE_MOVE(value, place) ((value) | (place << 4))
#define MOVE_VALUE(move) (move & 15)
#define MOVE_PLACE(move) (move >> 4)

struct Tile {
  int32_t value : 16;
  int32_t delta : 12;
  int32_t count : 4;
};

extern const struct Tile EMPTY_TILE;

enum Player { RED = 0, BLUE = 1 };

struct Position {
  enum Player player;
  uint8_t turns;
  uint16_t used_by[2];
  uint64_t filled;
  int32_t non_holes_count;
  struct Tile tiles[MAX_PLACES];
};

int32_t field_to_place(const char* field);

const char* place_to_field(int32_t place);

struct Position create_start_position();

void position_make_place_brown(struct Position* position, int32_t place);

void position_make_field_brown(struct Position* position, const char* field);

void position_init_tiles_count(struct Position* position);

void position_play(struct Position* position, int32_t value, int32_t place);

void position_play_move(struct Position* position, int32_t move);

double position_expected(const struct Position* position);

int32_t position_possible_values(const struct Position* position,
                                 int32_t values[MAX_VALUES]);

int32_t position_possible_places(const struct Position* position,
                                 int32_t places[MAX_PLACES]);

bool position_filling(const struct Position* position);

int32_t position_filling_move(const struct Position* position);

double position_playout(struct Position* position, int32_t selected_place);

bool position_hole(const struct Position* position, int32_t place);
