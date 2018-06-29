#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "common.h"

typedef struct linked_list_node {
  struct linked_list_node *prev;
  struct linked_list_node *next;
} linked_list_node;

typedef struct linked_list {
  linked_list_node sentinel;
  int size;
  size_t node_offset;
} linked_list;

typedef struct {
  linked_list_node *sentinel;
  linked_list_node *current;
} linked_list_cursor;

void linked_list_init(linked_list *l, size_t node_offset);
void linked_list_append(linked_list *l1, linked_list *l2);
void linked_list_insert_head(linked_list *l, void *item);
void linked_list_insert_tail(linked_list *l, void *item);
void *linked_list_head(linked_list *l);
void *linked_list_tail(linked_list *l);
void *linked_list_delete_head(linked_list *l);
void *linked_list_delete_tail(linked_list *l);
void *linked_list_delete(linked_list *l, void *item);
void *linked_list_search(linked_list *l, cmp_fn cmp, void *key);

#endif
