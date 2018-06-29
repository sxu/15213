#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include <stdbool.h>

#include "common.h"

typedef struct splay_tree_node {
  struct splay_tree_node *left;
  struct splay_tree_node *right;
} splay_tree_node;

typedef struct splay_tree {
  splay_tree_node *root;
  size_t size;
  size_t node_offset;
  cmp_fn cmp;
} splay_tree;

void splay_tree_init(splay_tree *t, size_t node_offset, cmp_fn cmp);
bool splay_tree_insert(splay_tree *t, void *item);
bool splay_tree_delete(splay_tree *t, void *item);
void *splay_tree_search(splay_tree *t, void *item);

#endif
