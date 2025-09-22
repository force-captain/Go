#include <stdint.h>
#include "states.h"
#include "board.h"
#include "list.h"

uint64_t state = 88172645463325252ULL;

List* previous_states = NULL;

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
	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < size; j++) {
			result ^= zobrist_table[i][j][get_state_colour(board, (Point){.x = i, .y = j})];
		}
	}
	return result;
}

int get_state_colour(Board* board, Point pt) {
	Tile* t = board_get_tile(board, pt);
	if (tile_get_colour(t) == NONE) return NONE;
	else if (tile_get_group(t) == NULL) return NONE;
	else if (group_is_captured(tile_get_group(t))) return NONE;
	else return tile_get_colour(t);
}

void init_states() {
	previous_states = list_init(sizeof(uint64_t), 16);
	for(size_t i = 0; i < MAX_SIZE; i++) {
		for(size_t j = 0; j < MAX_SIZE; j++) {
			for(int k = 0; k < 3; k++) {
				zobrist_table[i][j][k] = xorshift64();
			}
		}
	}
}

// Returns 1 if ko/superko (invalid move), returns 0 if valid
bool check_ko(Board* board) {
	uint64_t hash = get_hash(board);
	if (list_contains(previous_states, &hash)) {
		return true;
	} else {
		list_append(previous_states, &hash);
		return false;
	}
}
