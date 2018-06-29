#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

#define offsetof(type, member) ((size_t) &((type *)0)->member)

#define container_of(ptr, type, member) \
  ((type *) ((char *)(ptr) - offsetof(type, member)))

typedef int (*cmp_fn)(void *, void *);

#endif
