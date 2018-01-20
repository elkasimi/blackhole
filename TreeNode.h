#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "Position.h"

#define MAX_ACTIONS 36

enum TreeNodeType { VALUE, PLACE };

struct TreeNode {
  struct TreeNode* parent;
  struct TreeNode* next;
  struct TreeNode* first_child;
  enum Player player;
  int32_t action;
  enum TreeNodeType type;
  int32_t untried_count;
  int32_t untried_actions[MAX_ACTIONS];
  double value;
  int32_t visits;
};

void tree_nodes_store_init();

void tree_node_precompute();

struct TreeNode* tree_node_create_root(const struct Position* position);

struct TreeNode* tree_node_expand(struct TreeNode* node,
                                  struct Position* position);

bool tree_node_leaf(struct TreeNode* node);

struct TreeNode* tree_node_select(struct TreeNode* node);

struct TreeNode* tree_node_most_visited(struct TreeNode* node);

void tree_node_update(struct TreeNode* node, double value);

bool tree_node_fully_expanded(const struct TreeNode* node);
