#include <stddef.h>
#include <stdint.h>
#include "util/hash.h"
#include "board.h"
#include "point.h"
#include "groups.h"

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

int get_state_colour(Board* board, Point pt) {
    Tile* t = board_get_tile(board, pt);
    if (tile_get_colour(t) == NONE) return NONE;
    else if (tile_get_group(t) == NULL) return NONE;
    else if (group_is_captured(tile_get_group(t))) return NONE;
    else return tile_get_colour(t);
}

uint64_t get_hash(Board* board) {
	uint64_t result = 0;
	size_t size = board_get_size(board);
	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < size; j++) {
			result ^= zobrist_table[i][j][get_state_colour(board, (Point){i, j})];
		}
	}
    return result;
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
