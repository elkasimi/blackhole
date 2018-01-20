#include "MCTS.h"

#include "Position.h"
#include "Timer.h"
#include "TreeNode.h"

#include <stdio.h>

struct Result mcts_best_move(const struct Settings* settings) {
  const struct Position* position = settings->position;
  if (position_filling(position)) {
    struct Result result;

    result.move = position_filling_move(position);

    double e = position_expected(position), r = 75 + e, b = 75 - e;
    sprintf(result.log, "Using filling move, expected = (%.0f, %.0f)", r, b);

    result.delta_time = 0.0;

    return result;
  }

  tree_node_precompute();

  struct timeval start = timer_now();

  tree_nodes_store_init();

  int32_t max_iterations = settings->max_iterations;

  struct TreeNode* root = tree_node_create_root(position);
  int32_t iteration, max_level = 0;
  for (iteration = 0; iteration < max_iterations; ++iteration) {
    struct Position p = *position;
    struct TreeNode* node = root;
    struct TreeNode* first_move_node = NULL;

    while (tree_node_fully_expanded(node)) {
      node = tree_node_select(node);
      if (!tree_node_fully_expanded(node)) {
        break;
      }

      node = tree_node_select(node);
      position_play(&p, node->action, node->parent->action);
      if (first_move_node == NULL) {
        first_move_node = node;
      }
    }

    if (first_move_node && 2 * first_move_node->visits > max_iterations) {
      break;
    }

    if (!tree_node_leaf(node)) {
      node = tree_node_expand(node, &p);
    }

    int32_t level = p.turns - position->turns;
    max_level = max_level > level ? max_level : level;

    int32_t selected_place = node->type == PLACE ? node->action : -1;

    double score = position_playout(&p, selected_place);

    for (; node; node = node->parent) {
      tree_node_update(node, score);
      __builtin_prefetch(node->parent, 1, 1);
    }

    if (timer_delta_time_since(&start) >= settings->max_time) {
      break;
    }
  }

  struct Result result;

  struct TreeNode *place_node = tree_node_most_visited(root),
                  *value_node = tree_node_most_visited(place_node);
  result.move = CREATE_MOVE(value_node->action, place_node->action);

  sprintf(result.log, "i=%d (%.2f, %d) -> (%.2f, %d) l=%d", iteration,
          place_node->value, place_node->visits, value_node->value,
          value_node->visits, max_level);

  result.delta_time = timer_delta_time_since(&start);

  return result;
}
