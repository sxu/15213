#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cachelab.h"
#include "linked_list.h"
#include "splay_tree.h"

typedef enum {
  /* INST, */ // instruction load ignored
  LOAD,
  STORE,
  MODIFY,
} access_mode;

typedef struct mem_access {
  access_mode mode;
  uint64_t address;
  /* uint64_t num_bytes; */ /* not needed, assuming aligned access */
} mem_access;

typedef struct cache_line {
  uint64_t tag;
  linked_list_node ll_node;
  splay_tree_node st_node;
} cache_line;

typedef struct cache_set {
  cache_line *lines;
  linked_list ll;
  splay_tree st;
} cache_set;

void simulate(int num_set_bits, int num_block_bits, int associativity,
              char *trace_file_name, bool verbose);
bool nextAccess(FILE *trace_file, char *buffer, int buf_size,
                mem_access *access);
void decode(uint64_t address, int num_set_bits, int num_block_bits,
            uint64_t *set_idx, uint64_t *tag);
int cache_line_cmp(void *a, void *b);
void printHelp(char *argv0);

int main(int argc, char *argv[]) {
  bool verbose = false;
  int num_set_bits = 0;
  int num_block_bits = 0;
  int associativity = 0;
  char *trace_file_name = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0) {
      printHelp(argv[0]);
      return 0;
    } else if (strcmp(argv[i], "-v") == 0) {
      verbose = true;
    } else if (strcmp(argv[i], "-s") == 0) {
      if (++i == argc) {
        printf("%s: option requires an argument -- 's'\n", argv[0]);
        return 1;
      }
      num_set_bits = atoi(argv[i]);
    } else if (strcmp(argv[i], "-E") == 0) {
      if (++i == argc) {
        printf("%s: option requires an argument -- 'E'\n", argv[0]);
        printHelp(argv[0]);
        return 1;
      }
      associativity = atoi(argv[i]);
    } else if (strcmp(argv[i], "-b") == 0) {
      if (++i == argc) {
        printf("%s: option requires an argument -- 'b'\n", argv[0]);
        return 1;
      }
      num_block_bits = atoi(argv[i]);
    } else if (strcmp(argv[i], "-t") == 0) {
      if (++i == argc) {
        printf("%s: option requires an argument -- 't'\n", argv[0]);
        return 1;
      }
      trace_file_name = argv[i];
    }
  }

  if (num_set_bits <= 0 || num_block_bits <= 0 || associativity <= 0 ||
      trace_file_name == NULL) {
    printf("%s: Missing required command line argument\n", argv[0]);
    printHelp(argv[0]);
    return 1;
  }

  simulate(num_set_bits, num_block_bits, associativity, trace_file_name,
           verbose);

  return 0;
}

void simulate(int num_set_bits, int num_block_bits, int associativity,
              char *trace_file_name, bool verbose) {
  int num_sets = 1 << num_set_bits;
  cache_set *cache_sets = malloc(num_sets * sizeof(cache_set));
  if (!cache_sets) {
    printf("malloc failed");
    exit(1);
  }
  for (int i = 0; i < num_sets; i++) {
    cache_sets[i].lines = malloc(associativity * sizeof(cache_line));
    if (!cache_sets[i].lines) {
      printf("malloc failed");
      exit(1);
    }
    linked_list_initialize(&cache_sets[i].ll, offsetof(cache_line, ll_node));
    splay_tree_initialize(&cache_sets[i].st, offsetof(cache_line, st_node),
                          cache_line_cmp);
  }

  FILE *trace_file = fopen(trace_file_name, "r");
  if (!trace_file) {
    printf("Unable to open trace file: %s.\n", trace_file_name);
    exit(1);
  }

  int hits = 0;
  int misses = 0;
  int evictions = 0;
  mem_access access;
  cache_set *set;
  cache_line line, *hit;
  uint64_t tag;
  uint64_t set_idx;
  char buffer[50];
  while (nextAccess(trace_file, buffer, sizeof(buffer), &access)) {
    if (verbose) {
      printf(buffer + 1);
    }
    decode(access.address, num_set_bits, num_block_bits, &set_idx, &tag);
    assert(set_idx < num_sets);
    set = cache_sets + set_idx;
    line.tag = tag;
    hit = splay_tree_search(&set->st, &line);
    if (hit) {
      hits++;
      if (verbose) {
        printf(" hit");
      }
      linked_list_remove(&set->ll, hit);
      linked_list_push_front(&set->ll, hit);
      if (access.mode == MODIFY) {
        hits++;
        if (verbose) {
          printf(" hit");
        }
      }
    } else {
      misses++;
      if (verbose) {
        printf(" miss");
      }
      assert(set->st.size == set->ll.size);
      if (set->st.size < associativity) {
        cache_line *new_line = set->lines + set->st.size;
        new_line->tag = tag;
        linked_list_push_front(&set->ll, new_line);
        assert(splay_tree_insert(&set->st, new_line));
      } else {
        cache_line *new_line = (cache_line *)linked_list_pop_back(&set->ll);
        assert(splay_tree_remove(&set->st, new_line));
        new_line->tag = tag;
        linked_list_push_front(&set->ll, new_line);
        assert(splay_tree_insert(&set->st, new_line));
        evictions++;
        if (verbose) {
          printf(" eviction");
        }
      }
      if (access.mode == MODIFY) {
        hits++;
        if (verbose) {
          printf(" hit");
        }
      }
    }
    if (verbose) {
      printf("\n");
    }
  }
  printSummary(hits, misses, evictions);
}

int cache_line_cmp(void *a, void *b) {
  cache_line *la = (cache_line *)a;
  cache_line *lb = (cache_line *)b;
  if (la->tag == lb->tag) {
    return 0;
  } else if (la->tag < lb->tag) {
    return -1;
  } else {
    return 1;
  }
}

bool nextAccess(FILE *trace_file, char *buffer, int buf_size,
                mem_access *access) {
  char *got;
  for (;;) {
    got = fgets(buffer, buf_size, trace_file);
    if (!got) {
      break;
    }
    if (*got == ' ') {
      switch (*(++got)) {
      case 'L':
        access->mode = LOAD;
        break;
      case 'S':
        access->mode = STORE;
        break;
      case 'M':
        access->mode = MODIFY;
        break;
      default:
        printf("Unknown access mode: %c.\n", *got);
        exit(1);
      }
      assert(sscanf(++got, "%lx", &access->address) == 1);
      break;
    }
    assert(*got == 'I');
  }
  while (*buffer != '\0' && *buffer != '\n') {
    buffer++;
  }
  if (*buffer == '\n') {
    *buffer = '\0';
  }
  return got != NULL;
}

void decode(uint64_t address, int num_set_bits, int num_block_bits,
            uint64_t *set, uint64_t *tag) {
  int64_t sign_bit = 1UL << 63;
  address &= ~sign_bit;
  address >>= num_block_bits;
  *set = address & ~(sign_bit >> (63 - num_set_bits));
  *tag = address >> num_set_bits;
}

void printHelp(char *argv0) {
  printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv0);
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n\n");
  printf("Examples:\n");
  printf("linux> %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv0);
  printf("linux> %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv0);
}
