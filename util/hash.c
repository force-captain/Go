#include <stddef.h>
#include <stdint.h>
#include "hash.h"
#include "../board.h"

uint64_t state = 88172345361325652ULL;

uint64_t zobrist_table[MAX_SIZE][MAX_SIZE][3];

uint64_t xorshift64() {
	uint64_t x = state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	state = x;
	return x;
}

uint64_t get_hash(Board* board) {
	uint64_t result = 0;
	int size board->size;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result ^= zobrist_table[i][j][board->data[i][j].Colour]
		}
	}
}

void init_states() {
	for(int i = 0; i < MAX_SIZE; i++) {
		for(int j = 0; j < MAX_SIZE; j++) {
			for (int k = 0; k < 3; k++) {
				zobrist_table[i][j][k] = xorshift64();
			}
		}
	}
}
