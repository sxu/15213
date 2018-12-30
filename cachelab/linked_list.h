#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

typedef struct linked_list_node {
  struct linked_list_node *prev;
  struct linked_list_node *next;
} linked_list_node;

typedef struct linked_list {
  linked_list_node sentinel;
  size_t size;
  size_t node_offset;
} linked_list;

typedef struct {
  linked_list_node *sentinel;
  linked_list_node *current;
} linked_list_cursor;

void linked_list_initialize(linked_list *l, size_t node_offset);
void linked_list_append(linked_list *l1, linked_list *l2);
void linked_list_push_front(linked_list *l, void *item);
void linked_list_push_back(linked_list *l, void *item);
void *linked_list_pop_front(linked_list *l);
void *linked_list_pop_back(linked_list *l);
void *linked_list_remove(linked_list *l, void *item);
void *linked_list_front(linked_list *l);
void *linked_list_back(linked_list *l);
void *linked_list_find(linked_list *l, int (*cmp)(void *, void *), void *key);

#endif
