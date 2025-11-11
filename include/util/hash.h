#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

#define MAX_SIZE 19

typedef struct Board Board;

uint64_t get_hash(Board* board);

#endif
