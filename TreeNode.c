#include "TreeNode.h"

#include "Position.h"
#include "Random.h"

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#define INVALID_ACTION -1

#define MAX_ITERATIONS 100000
#define MAX_VISITS MAX_ITERATIONS

static double stored_sqrt[MAX_VISITS + 1], stored_sqrt_of_log[MAX_VISITS + 1];

void tree_node_precompute() {
  static bool done = false;

  if (done) {
    return;
  }

  for (int32_t i = 1; i <= MAX_VISITS; ++i) {
    double fi = (double)i;
    stored_sqrt[i] = sqrt(fi);
    stored_sqrt_of_log[i] = sqrt(log(fi));
  }

  done = true;
}

#define MAX_TREE_NODES (1 + 2 * MAX_ITERATIONS)

static struct TreeNodeStore {
  struct TreeNode* current;
  struct TreeNode available_nodes[MAX_TREE_NODES];
} tree_nodes_store;

void tree_nodes_store_init() {
  tree_nodes_store.current = &tree_nodes_store.available_nodes[0];
}

static struct TreeNode* tree_nodes_allocate() {
  assert(tree_nodes_store.current !=
         tree_nodes_store.available_nodes + MAX_TREE_NODES);
  return tree_nodes_store.current++;
}

static enum TreeNodeType tree_node_type(struct TreeNode* parent) {
  return parent == NULL ? VALUE : parent->type == PLACE ? VALUE : PLACE;
}

static bool color(enum TreeNodeType type, enum Player player) {
  return type == PLACE ? player : 1 - player;
}

static int32_t tree_node_possible_actions(struct TreeNode* node,
                                          const struct Position* position) {
  if (position_filling(position)) {
    return 0;
  }

  if (node->type == PLACE) {
    int32_t count = position_possible_values(position, node->untried_actions);
    int32_t place = node->action;
    return position_hole(position, place) ? 1 : count;
  } else {
    return position_possible_places(position, node->untried_actions);
  }
}

static void tree_node_init(struct TreeNode* node, struct TreeNode* parent,
                           int32_t action, const struct Position* position) {
  node->parent = parent;
  node->next = NULL;
  node->first_child = NULL;
  node->action = action;
  node->type = tree_node_type(parent);
  node->player = color(node->type, position->player);
  node->untried_count = tree_node_possible_actions(node, position);
  node->value = 0.0;
  node->visits = 0;
}

struct TreeNode* tree_node_create_root(const struct Position* position) {
  struct TreeNode* root = tree_nodes_allocate();

  tree_node_init(root, NULL, INVALID_ACTION, position);

  return root;
}

struct TreeNode* tree_node_expand(struct TreeNode* node,
                                  struct Position* position) {
  int32_t i = random_int(node->untried_count);
  int32_t action = node->untried_actions[i];
  node->untried_count--;
  node->untried_actions[i] = node->untried_actions[node->untried_count];

  if (node->type == PLACE) {
    position_play(position, action, node->action);
  }

  struct TreeNode* child = tree_nodes_allocate();
  tree_node_init(child, node, action, position);
  child->next = node->first_child;
  node->first_child = child;

  return child;
}

bool tree_node_leaf(struct TreeNode* node) {
  return node->untried_count == 0 && node->first_child == NULL;
}

#define FOR_EACH_CHILD(node, child)            \
  __builtin_prefetch(node->first_child, 0, 1); \
  for (child = node->first_child; child; child = child->next)

static struct TreeNode* tree_node_random(struct TreeNode* node) {
  struct TreeNode *child, *children[MAX_ACTIONS];
  int32_t count = 0;
  FOR_EACH_CHILD(node, child) {
    children[count++] = child;
    __builtin_prefetch(child->next, 0, 1);
  }

  return children[random_int(count)];
}

static struct TreeNode* tree_node_best(struct TreeNode* node) {
  double best_value = -DBL_MAX;
  struct TreeNode *child, *best = NULL;
  FOR_EACH_CHILD(node, child) {
    if (best_value < child->value) {
      best_value = child->value;
      best = child;
    }

    __builtin_prefetch(child->next, 0, 1);
  }

  return best;
}

static struct TreeNode* tree_node_epsilon_greedy(struct TreeNode* node) {
  return random_int(10) != 0 ? tree_node_best(node) : tree_node_random(node);
}

#if 0
static struct TreeNode* tree_node_most_promising(struct TreeNode* node) {
  double best_value = -DBL_MAX;
  struct TreeNode *best_node = NULL, *child, *children[MAX_ACTIONS];

  int32_t n = node->visits;
  int32_t count = 0;
  FOR_EACH_CHILD(node, child) {
    children[count++] = child;
    double vi = child->value;
    if (!tree_node_leaf(child)) {
      int32_t ni = child->visits;
      double exploration_bonus = 2.0 * stored_sqrt_of_log[n] / stored_sqrt[ni];
      vi += exploration_bonus;
    }

    if (best_value < vi) {
      best_value = vi;
      best_node = child;
    }

    __builtin_prefetch(child->next, 0, 1);
  }

  return best_value > -20.0 ? best_node : children[random_int(count)];
}
#endif

struct TreeNode* tree_node_select(struct TreeNode* node) {
  return tree_node_epsilon_greedy(node);
}

struct TreeNode* tree_node_most_visited(struct TreeNode* node) {
  int32_t max_visits = INT_MIN;
  struct TreeNode *most_visited = NULL, *child;

  FOR_EACH_CHILD(node, child) {
    if (max_visits < child->visits) {
      max_visits = child->visits;
      most_visited = child;
    }
  }

  return most_visited;
}

void tree_node_update(struct TreeNode* node, double value) {
  if (node->player == BLUE) {
    value = -value;
  }

  node->value = node->value * node->visits + value;
  node->visits++;
  node->value /= node->visits;
}

bool tree_node_fully_expanded(const struct TreeNode* node) {
  return node->untried_count == 0 && node->first_child != NULL;
}
