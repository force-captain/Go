#include <stdint.h>
#include "states.h"
#include "board.h"
#include "list.h"

uint64_t state = 88172645463325252ULL;

List previous_states;

uint64_t xorshift64() {
	uint64_t x = state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	state = x;
	return x;
}

uint64_t zobrist_table[MAX_SIZE][MAX_SIZE][3];

uint64_t get_hash(Board* board) {
	uint64_t result = 0;
	int size = board->size;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result ^= zobrist_table[i][j][board->data[i][j]];
		}
	}
	return result;
}

void init_states() {
	for(int i = 0; i < MAX_SIZE; i++) {
		for(int j = 0; j < MAX_SIZE; j++) {
			for(int k = 0; k < 3; k++) {
				zobrist_table[i][j][k] = xorshift64();
			}
		}
	}
}

// Returns 1 if ko/superko (invalid move), returns 0 if valid
extern int check_ko(Board* board) {
	uint64_t hash = get_hash(board);
	if (list_contains(&previous_states, &hash) == 1) {
		return 1;
	} else {
		list_append(&previous_states, &hash);
		return 0;
	}
}
