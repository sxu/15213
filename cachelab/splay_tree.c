#include <stdio.h>
#include <assert.h>
#include "splay_tree.h"

/* Daniel Sleator's top-down splay */
static splay_tree_node *splay(splay_tree_node *n, size_t offset, cmp_fn cmp, void *key) {
  splay_tree_node N, *l, *r, *x;

  assert(n);
  N.left = N.right = NULL;
  l = r = &N;
  for (;;) {
    if (cmp(key, (char *)n - offset) < 0) {
      if (!n->left) break;
      if (cmp(key, (char *)n->left - offset) < 0) {
        x = n->left;
        n->left = x->right;
        x->right = n;
        n = x;
        if (!n->left) break;
      }
      r->left = n;
      r = n;
      n = n->left;
      assert(n);
    } else if (cmp(key, (char *)n - offset) > 0) {
      if (!n->right) break;
      if (cmp(key, (char *)n->right - offset) > 0) {
        x = n->right;
        n->right = x->left;
        x->left = n;
        n = x;
        if (!n->right) break;
      }
      l->right = n;
      l = n;
      n = n->right;
      assert(n);
    } else {
      break;
    }
  }
  l->right = n->left;
  r->left = n->right;
  n->left = N.right;
  n->right = N.left;
  assert(n != NULL);
  return n;
}

void splay_tree_init(splay_tree *t, size_t node_offset, cmp_fn cmp) {
  t->root = NULL;
  t->size = 0;
  t->node_offset = node_offset;
  t->cmp = cmp;
}

bool splay_tree_insert(splay_tree *t, void *item) {
  splay_tree_node *node = (splay_tree_node *)((char *)item + t->node_offset);
  splay_tree_node *x;

  if (!t->root) {
    node->left = node->right = NULL;
    t->root = node;
    t->size++;
    return true;
  }

  x = splay(t->root, t->node_offset, t->cmp, item);
  t->root = x;
  if (t->cmp(item, (char *)x - t->node_offset) < 0) {
    node->left = x->left;
    node->right = x;
    x->left = NULL;
    t->root = node;
    t->size++;
    return true;
  } else if (t->cmp(item, (char *)x - t->node_offset) > 0) {
    node->right = x->right;
    node->left = x;
    x->right = NULL;
    t->root = node;
    t->size++;
    return true;
  } else {
    return false;
  }
}

bool splay_tree_delete(splay_tree *t, void *item) {
  splay_tree_node *x, *y;

  if (!t->root) {
    return false;
  }
  x = splay(t->root, t->node_offset, t->cmp, item);
  if (t->cmp(item, (char *)x - t->node_offset) == 0) {
    if (!x->left) {
      y = x->right;
    } else {
      y = splay(x->left, t->node_offset, t->cmp, item);
      y->right = x->right;
    }
    t->size--;
    t->root = y;
    return true;
  }
  return false;
}

void *splay_tree_search(splay_tree *t, void *item) {
  if (!t->root) {
    return NULL;
  }
  splay_tree_node *n = splay(t->root, t->node_offset, t->cmp,  item);
  t->root = n;
  if (t->cmp(item, (char *)n - t->node_offset) == 0) {
    return (char *)n - t->node_offset;
  }
  return NULL;
}

// static void st_map(splay_tree_node *n, map_fn op) {
//   if (!n) return;
//   st_map(n->left, op);
//   op(n->stuff);
//   st_map(n->right, op);
// }
// 
// void *splay_tree_map(splay_tree *t, map_fn op)
// {
//   if (!t->root) return;
//   else st_map(t->root, op);
// }
// 
// static void st_foldl(splay_tree_node *n, fold_fn combine, void *curr)
// {
//   if (!n) return;
//   st_foldl(n->left, combine, curr);
//   combine(curr, n->stuff);
//   st_foldl(n->right, combine, curr);
// }
// 
// void *splay_tree_foldl(splay_tree *t, fold_fn combine, void *curr)
// {
//   if (!t->root) return;
//   else st_foldl(t->root, combine, curr);
// }

/*
static void rotate_left(splay_tree_node *node)
{
  splay_tree_node *y = x->right;

  y->parent = x->parent;
  if (x->parent) {
    if (x == x->parent->left) {
      x->parent->left = y;
    } else {
      x->parent->right = y;
    }
  }
  x->parent = y;
  x->right = y->left;
  if (y->left) y->left->parent = x;
}

static void rotate_right(splay_tree_node *x)
{
  splay_tree_node *y = x->left;

  y->parent = x->parent;
  if (x->parent) {
    if (x == x->parent->left) {
      x->parent->left = y;
    } else {
      x->parent->right = y;
    }
  }
  x->parent = y;
  x->left = y->right;
  if (y->right) y->right->parent = x;
}

static void splay(splay_tree_node *node)
{
  while (node->parent) {
    if (node->parent->parent == NULL) {
      if (node == node->parent->left) {
        rotate_right(node->parent);
      } else {
        rotate_left(node->parent);
      }
    } else if (node == node->parent->left && node->parent == node->parent->parent->left) {
      rotate_right(node->parent->parent);
      rotate_right(node->parent);
    } else if (node == node->parent->right && node->parent == node->parent->parent->right) {
      rotate_left(node->parent->parent);
      rotate_left(node->parent);
    } else if (node == node->parent->left && node->parent == node->parent->parent->right) {
      rotate_right(node->parent);
      rotate_left(node->parent);
    } else {
      rotate_left(node->parent);
      rotate_right(node->parent);
    }
  }
}
*/
