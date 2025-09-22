#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

#define MAX_SIZE 19

typedef struct Board Board;

extern uint64_t state;
extern uint64_t zobrist_table[MAX_SIZE][MAX_SIZE][3];
uint64_t xorshift64(void);
extern uint64_t get_hash(Board* board);
void init_states(void);

#endif
