#include "Position.h"

#include "Random.h"

#include <assert.h>
#include <string.h>

#define MAX_PLACE 36
#define MAX_NEIGHBORS 7
#define MIN_VALUE 1
#define MAX_VALUE 15
#define MAX_VALUES 15
#define MAX_PLACES 36

#define FOR(i, n) \
  int32_t i;      \
  for (i = 0; i < (n); ++i)

#define FOR_EACH_PLACE(place) \
  int32_t place;              \
  for (place = 0; place < MAX_PLACE; ++place)

#define FOR_EACH_NEIGHBOR(neighbor, place) \
  const int32_t* neighbor;                 \
  for (neighbor = NEIGHBORS[place]; *neighbor != -1; ++neighbor)

#define FOR_EACH_VALUE(value) \
  int32_t value;              \
  for (value = MIN_VALUE; value <= MAX_VALUE; ++value)

static const int32_t NEIGHBORS[MAX_PLACE][MAX_NEIGHBORS] = {
    {1, 2, -1},
    {0, 2, 3, 4, -1},
    {0, 1, 4, 5, -1},
    {1, 4, 6, 7, -1},
    {1, 2, 3, 5, 7, 8, -1},
    {2, 4, 8, 9, -1},
    {3, 7, 10, 11, -1},
    {3, 4, 6, 8, 11, 12, -1},
    {4, 5, 7, 9, 12, 13, -1},
    {5, 8, 13, 14, -1},
    {6, 11, 15, 16, -1},
    {6, 7, 10, 12, 16, 17, -1},
    {7, 8, 11, 13, 17, 18, -1},
    {8, 9, 12, 14, 18, 19, -1},
    {9, 13, 19, 20, -1},
    {10, 16, 21, 22, -1},
    {10, 11, 15, 17, 22, 23, -1},
    {11, 12, 16, 18, 23, 24, -1},
    {12, 13, 17, 19, 24, 25, -1},
    {13, 14, 18, 20, 25, 26, -1},
    {14, 19, 26, 27, -1},
    {15, 22, 28, 29, -1},
    {15, 16, 21, 23, 29, 30, -1},
    {16, 17, 22, 24, 30, 31, -1},
    {17, 18, 23, 25, 31, 32, -1},
    {18, 19, 24, 26, 32, 33, -1},
    {19, 20, 25, 27, 33, 34, -1},
    {20, 26, 34, 35, -1},
    {21, 29, -1},
    {21, 22, 28, 30, -1},
    {22, 23, 29, 31, -1},
    {23, 24, 30, 32, -1},
    {24, 25, 31, 33, -1},
    {25, 26, 32, 34, -1},
    {26, 27, 33, 35, -1},
    {27, 34, -1}};

static const char* FIELDS[] = {
    "A1", "B1", "A2", "C1", "B2", "A3", "D1", "C2", "B3", "A4", "E1", "D2",
    "C3", "B4", "A5", "F1", "E2", "D3", "C4", "B5", "A6", "G1", "F2", "E3",
    "D4", "C5", "B6", "A7", "H1", "G2", "F3", "E4", "D5", "C6", "B7", "A8"};

const struct Tile EMPTY_TILE = {.value = 0, .delta = 0, .count = 0};

struct Position create_start_position() {
  struct Position start_position;
  start_position.player = RED;
  start_position.turns = 0;
  start_position.used_by[0] = 0u;
  start_position.used_by[1] = 0u;
  start_position.filled = 0ull;
  start_position.non_holes_count = 0;
  FOR_EACH_PLACE(place) { start_position.tiles[place] = EMPTY_TILE; }

  return start_position;
}

int32_t field_to_place(const char* field) {
  FOR_EACH_PLACE(place) {
    if (strcmp(FIELDS[place], field) == 0) {
      return place;
    }
  }

  assert(false);
  return -1;
}

const char* place_to_field(int32_t place) {
  assert(place >= 0 && place < MAX_PLACE);
  return FIELDS[place];
}

static bool position_filled(const struct Position* position, int32_t place) {
  return (position->filled & (1ull << place)) != 0;
}

static bool position_used(const struct Position* position, int32_t value) {
  return (position->used_by[position->player] & (1 << value)) != 0;
}

void position_make_place_brown(struct Position* position, int32_t place) {
  position->filled |= 1ull << place;
}

void position_make_field_brown(struct Position* position, const char* field) {
  int32_t place = field_to_place(field);
  position_make_place_brown(position, place);
}

void position_init_tiles_count(struct Position* position) {
  FOR_EACH_PLACE(place) {
    FOR_EACH_NEIGHBOR(neighbor, place) {
      position->tiles[place].count += !position_filled(position, *neighbor);
    }
  }

  position->non_holes_count = 0;
  FOR_EACH_PLACE(p) {
    position->non_holes_count +=
        !position_filled(position, p) && (position->tiles[p].count != 0);
  }
}

void position_play(struct Position* position, int32_t value, int32_t place) {
  position->filled |= 1ull << place;

  position->used_by[position->player] |= 1 << value;

  if (position->player == BLUE) {
    value = -value;
  }

  struct Tile* tiles = position->tiles;
  tiles[place].value = value;

  if (tiles[place].count != 0) {
    position->non_holes_count--;
  }

  FOR_EACH_NEIGHBOR(neighbor, place) {
    struct Tile* tile = &tiles[*neighbor];
    tile->delta += value;
    tile->count--;

    if (!position_filled(position, *neighbor) && tile->count == 0) {
      position->non_holes_count--;
    }
  }

  position->player = position->player == RED ? BLUE : RED;

  position->turns++;
}

void position_play_move(struct Position* position, int32_t move) {
  position_play(position, MOVE_VALUE(move), MOVE_PLACE(move));
}

double position_expected(const struct Position* position) {
  int32_t count = 0, values[MAX_PLACES];
  FOR_EACH_PLACE(place) {
    if (!position_filled(position, place)) {
      values[count++] = position->tiles[place].delta;
    }
  }

  int32_t i, j;
  for (i = 1; i < count; ++i) {
    for (j = i; j > 0 && values[j - 1] > values[j]; --j) {
      int32_t t = values[j - 1];
      values[j - 1] = values[j];
      values[j] = t;
    }
  }

  int32_t index = (count - 1) / 2;
  int32_t value = values[index];
  int32_t sum = 0, sum_count;
  if (value < 0) {
    for (int32_t i = 0; i <= index; ++i) {
      sum += values[i];
    }
    sum_count = index + 1;
  } else if (value > 0) {
    for (int32_t i = index; i < count; ++i) {
      sum += values[i];
    }
    sum_count = count - index;
  } else {
    sum_count = count;
  }

  double average = ((double)sum) / sum_count;
  double average_weight = 0.05 * value * value;

  return (value + average_weight * average) / (1.0 + average_weight);
}

int32_t position_possible_values(const struct Position* position,
                                 int32_t values[MAX_VALUES]) {
  int32_t count = 0;
  FOR_EACH_VALUE(value) {
    if (!position_used(position, value)) {
      values[count++] = value;
    }
  }

  return count;
}

int32_t position_possible_places(const struct Position* position,
                                 int32_t places[MAX_PLACES]) {
  int32_t count = 0;
  FOR_EACH_PLACE(place) {
    if (!position_filled(position, place)) {
      places[count++] = place;
    }
  }

  return count;
}

bool position_filling(const struct Position* position) {
  return position->non_holes_count == 0;
}

int32_t position_filling_move(const struct Position* position) {
  int32_t best = 1000;
  int32_t best_place = -1;

  FOR_EACH_PLACE(place) {
    if (position_filled(position, place)) {
      continue;
    }

    const struct Tile* tile = &position->tiles[place];
    int32_t value = position->player == RED ? tile->delta : -tile->delta;
    if (best > value) {
      best = value;
      best_place = place;
    }
  }

  FOR_EACH_VALUE(value) {
    if (!position_used(position, value)) {
      return CREATE_MOVE(value, best_place);
    }
  }

  return INVALID_MOVE;
}

static int32_t delta_metric(int32_t count, int32_t delta) {
  return count + (delta << 10);
}

static int32_t count_metric(int32_t count, int32_t delta) {
  return (count << 10) + delta;
}

static int32_t position_pick(const struct Position* position, int32_t places[],
                             int32_t places_count,
                             int32_t (*metric)(int32_t, int32_t)) {
  int32_t places_indices[100], c = 0, best = -1000000;
  FOR(i, places_count) {
    int32_t place = places[i];
    const struct Tile* tile = &position->tiles[place];
    int32_t delta = position->player == RED ? -tile->delta : tile->delta;
    int32_t count = tile->count;
    int32_t value = metric(count, delta);
    if (best < value) {
      best = value;
      c = 0;
    }

    if (best == value) {
      places_indices[c++] = i;
    }
  }

  assert(c != 0);

  int32_t r = random_int(c);
  int32_t index = places_indices[r];
  int32_t good_place = places[index];
  places[index] = places[places_count - 1];

  return good_place;
}

struct Values {
  int32_t data[MAX_VALUES];
  int32_t min_index;
  int32_t max_index;
};

static int32_t pick_min(struct Values* values) {
  assert(values->min_index != MAX_VALUES);
  return values->data[values->min_index++];
}

static int32_t pick_max(struct Values* values) {
  assert(values->max_index >= 0);
  return values->data[values->max_index--];
}

static void position_values(const struct Position* position, enum Player player,
                            struct Values* values) {
  int32_t count = 0;
  FOR_EACH_VALUE(value) {
    if (!(position->used_by[player] & (1 << value))) {
      values->data[count++] = value;
    }
  }

  values->min_index = 0;
  values->max_index = count - 1;
}

bool position_hole(const struct Position* position, int32_t place) {
  return position->tiles[place].count == 0;
}

double position_playout(struct Position* position, int32_t selected_place) {
  if (position_filling(position)) {
    return position_expected(position);
  }

  int32_t places[MAX_PLACES];
  int32_t places_count = position_possible_places(position, places);
  struct Values values[2];
  position_values(position, RED, &values[RED]);
  position_values(position, BLUE, &values[BLUE]);

  int32_t place = selected_place;
  if (place != -1) {
    FOR(i, places_count) {
      if (places[i] == place) {
        places[i] = places[places_count - 1];
      }
    }
  }

  for (; places_count > 1; --places_count) {
    if (position_filling(position)) {
      return position_expected(position);
    }

    if (place != -1) {
    } else if (position->turns <= random_int(MAX_TURNS)) {
      place = position_pick(position, places, places_count, &count_metric);
    } else {
      place = position_pick(position, places, places_count, &delta_metric);
    }

    struct Values* player_values = &values[position->player];
    int32_t value = position_hole(position, place) ? pick_min(player_values)
                                                   : pick_max(player_values);

    position_play(position, value, place);
    place = -1;
  }

  int32_t hole = places[0];
  return position->tiles[hole].delta;
}
