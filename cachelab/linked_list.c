#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"

static inline void insert_after(linked_list_node *node, linked_list_node *new) {
  new->prev = node;
  new->next = node->next;
  new->prev->next = new;
  new->next->prev = new;
}

static inline linked_list_node *remove_node(linked_list_node *node) {
  if (!node)
    return NULL;

  node->prev->next = node->next;
  node->next->prev = node->prev;
  return node;
}

void linked_list_initialize(linked_list *l, size_t node_offset) {
  l->size = 0;
  l->node_offset = node_offset;
  l->sentinel.prev = l->sentinel.next = &l->sentinel;
}

void linked_list_append(linked_list *l1, linked_list *l2) {
  if (l2->size > 0) {
    if (l1->size > 0) {
      l1->sentinel.prev->next = l2->sentinel.next;
      l2->sentinel.next->prev = l1->sentinel.prev;
      l1->sentinel.prev = l2->sentinel.prev;
      l2->sentinel.prev->next = &l1->sentinel;
    } else {
      l1->sentinel.next = l2->sentinel.next;
      l1->sentinel.next->prev = &l1->sentinel;
      l1->sentinel.prev = l2->sentinel.prev;
      l1->sentinel.prev->next = &l1->sentinel;
    }
    l1->size += l2->size;
  }
}

void linked_list_push_front(linked_list *l, void *item) {
  linked_list_node *node = (linked_list_node *)((char *)item + l->node_offset);
  insert_after(&l->sentinel, node);
  l->size++;
}

void linked_list_push_back(linked_list *l, void *item) {
  linked_list_node *node = (linked_list_node *)((char *)item + l->node_offset);
  insert_after(l->sentinel.prev, node);
  l->size++;
  assert(l->sentinel.prev = node);
  assert(node->next = &l->sentinel);
}

void *linked_list_front(linked_list *l) {
  return (char *)l->sentinel.next - l->node_offset;
}

void *linked_list_back(linked_list *l) {
  return (char *)l->sentinel.prev - l->node_offset;
}

void *linked_list_pop_front(linked_list *l) {
  assert(l->size > 0);
  linked_list_node *head = remove_node(l->sentinel.next);
  l->size--;
  return (char *)head - l->node_offset;
}

void *linked_list_pop_back(linked_list *l) {
  assert(l->size > 0);
  linked_list_node *tail = remove_node(l->sentinel.prev);
  l->size--;
  return (char *)tail - l->node_offset;
}

void *linked_list_remove(linked_list *l, void *item) {
  assert(l->size > 0);
  l->size--;
  remove_node((linked_list_node *)((char *)item + l->node_offset));
  return item;
}

void *linked_list_find(linked_list *l, int (*cmp)(void *, void *), void *key) {
  for (linked_list_node *node = l->sentinel.next; node != &l->sentinel;
       node = node->next) {
    void *item = (char *)node - l->node_offset;
    if (cmp(key, item) == 0) {
      return item;
    }
  }
  return NULL;
}
