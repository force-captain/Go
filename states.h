#ifndef BOARD_STATES_H
#define BOARD_STATES_H

#include <stddef.h>
#include <stdint.h>

#define MAX_SIZE 19

typedef struct Board Board;
typedef struct List List;

extern uint64_t state;
extern List previous_states;
extern uint64_t zobrist_table[MAX_SIZE][MAX_SIZE][3];
uint64_t xorshift64(void);

extern uint64_t get_hash(Board* board);

void init_states(void);

extern int check_ko(Board* board);

#endif
