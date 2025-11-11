#include <stdint.h>
#include "states.h"
#include "groups.h"
#include "board.h"
#include "util/list.h"
#include "util/hash.h"

List* previous_states = NULL;

int uintptr_cmp(uint64_t* a, uint64_t* b) {
    return *b - *a;
}

void init_states() {
    previous_states = list_init(sizeof(uint64_t), 128, NULL);
}

// Returns 1 if ko/superko (invalid move), returns 0 if valid
bool check_ko(Board* board) {
    if (previous_states == NULL) {
        init_states();
    }
	uint64_t hash = get_hash(board);
	if (list_contains(previous_states, &hash) != -1) {
		return true;
	} else {
		list_append(previous_states, &hash);
		return false;
	}
}
