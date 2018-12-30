#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct splay_tree_node {
  struct splay_tree_node *left;
  struct splay_tree_node *right;
} splay_tree_node;

typedef struct splay_tree {
  splay_tree_node *root;
  size_t size;
  size_t node_offset;
  int (*cmp)(void *, void *);
} splay_tree;

void splay_tree_initialize(splay_tree *t, size_t node_offset,
                           int (*cmp)(void *, void *));
bool splay_tree_insert(splay_tree *t, void *item);
bool splay_tree_remove(splay_tree *t, void *item);
void *splay_tree_search(splay_tree *t, void *item);

#endif
