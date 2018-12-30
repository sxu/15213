#include "splay_tree.h"
#include <assert.h>
#include <stdio.h>

/* Daniel Sleator's top-down splay */
static splay_tree_node *splay(splay_tree_node *n, size_t offset,
                              int (*cmp)(void *, void *), void *key) {
  splay_tree_node tmp, *l, *r, *x;
  assert(n);
  tmp.left = tmp.right = NULL;
  l = r = &tmp;
  for (;;) {
    if (cmp(key, (char *)n - offset) < 0) {
      if (!n->left) {
        break;
      }
      if (cmp(key, (char *)n->left - offset) < 0) {
        x = n->left;
        n->left = x->right;
        x->right = n;
        n = x;
        if (!n->left) {
          break;
        }
      }
      r->left = n;
      r = n;
      n = n->left;
      assert(n);
    } else if (cmp(key, (char *)n - offset) > 0) {
      if (!n->right) {
        break;
      }
      if (cmp(key, (char *)n->right - offset) > 0) {
        x = n->right;
        n->right = x->left;
        x->left = n;
        n = x;
        if (!n->right) {
          break;
        }
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
  n->left = tmp.right;
  n->right = tmp.left;
  assert(n != NULL);
  return n;
}

void splay_tree_initialize(splay_tree *t, size_t node_offset,
                           int (*cmp)(void *, void *)) {
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

bool splay_tree_remove(splay_tree *t, void *item) {
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
  splay_tree_node *n = splay(t->root, t->node_offset, t->cmp, item);
  t->root = n;
  if (t->cmp(item, (char *)n - t->node_offset) == 0) {
    return (char *)n - t->node_offset;
  }
  return NULL;
}
