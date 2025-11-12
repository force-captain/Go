#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "util/list.h"
#include "groups.h"
#include "board.h"
#include "states.h"

typedef struct Tile {
	Colour colour;
	Group* group;
} Tile;

typedef struct Board {
	uint8_t size;
	Tile data[MAX_SIZE][MAX_SIZE];
	List* groups;
} Board;

Board* board_init(uint8_t size) {
	Board* b = malloc(sizeof(Board));
	if (!b) {
		return NULL;
	}
	b->size = size;

	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			b->data[i][j] = (Tile){ .colour = NONE, .group = NULL };
		}
	}
	b->groups = list_init(sizeof(Group*), 4, NULL);

	return b;
}

Colour board_get_colour(Board* b, Point pt) {
	return b->data[pt.x][pt.y].colour;
}

void board_append_group(Board* b, Group* g) {
	list_append(b->groups, g);
}

uint8_t board_get_size(Board* b) {
    return b->size;
}

Tile* board_get_tile(Board* b, Point pt) {
	return &b->data[pt.x][pt.y];
}

Group* tile_get_group(Tile* t) {
	return t->group;
}

Colour tile_get_colour(Tile* t) {
	return t->colour;
}

void tile_set_group(Tile* t, Group* g) {
	if (g == NULL) {
		t->group = NULL;
		t->colour = NONE;
	} else {
		t->group = g;
		t->colour = group_get_colour(g);
	}
}

void board_free(Board* board) {
	if (!board) {
		return;
	}
	list_free(board->groups, group_free_elem);
	free(board);
}


int board_set_tile(Board* b, Colour colour, Point pt) {
	if (point_in_bounds(b->size, pt)) {
		b->data[pt.x][pt.y].colour = colour;
		return MOVE_OK;
	}
	return MOVE_INVALID;
}

void board_get_neighbours(Point neighbours[4], Point pt) {
	neighbours[0] = (Point){ pt.x - 1, pt.y };
	neighbours[1] = (Point){ pt.x + 1, pt.y };
	neighbours[2] = (Point){ pt.x, pt.y - 1 };
	neighbours[3] = (Point){ pt.x, pt.y + 1 };
}

int board_add_group(Board *b, Group *g) { 
    List* groups = b->groups;
    if (!groups) {
        b->groups = list_init(sizeof(Group*), 8, NULL);
        groups = b->groups;
    }

    list_append(groups, g);
    return 0;
}

int board_place_tile(Board* board, Colour colour, Point pt) {
    // check point validity
	if (point_in_bounds(board->size, pt) == 0 || board_get_colour(board, pt) != NONE) {
		return MOVE_INVALID;
	}

    // create and add new group
    Tile* t = board_get_tile(board, pt);
    Group* newGroup = group_init(pt, colour);
    tile_set_group(t, newGroup);

    board_add_group(board, newGroup);

	Point neighbours[4];
	board_get_neighbours(neighbours, pt);

    // update enemy liberties
	for(int i = 0; i < 4; i++) {
		Point n = neighbours[i];
		if (!point_in_bounds(board->size, n)) continue;

		Tile* n_t = board_get_tile(board, n);
        if (!n_t || n_t->colour == NONE) continue;

        Group* g = n_t->group;
        if (group_get_colour(g) != colour) {
            group_remove_liberty(g, &pt);
        }
    }

    // mark capture
    for (int i = 0; i < 4; i++) {
        Point n = neighbours[i];
        if (!point_in_bounds(board->size, n)) continue;

        Tile* nt = board_get_tile(board, n);
        if (!nt || nt->colour == NONE) continue;

        Group* g = nt->group;
        if (g && group_get_colour(g) != colour && group_get_liberty_count(g) == 0) {
            group_mark_for_capture(g, true);
        }
    }
    
    // check for suicide
    group_update_liberties(board, newGroup);
    if (group_get_liberty_count(newGroup) == 0) {
        bool anyCaptured = false;
        List* groups = board_get_groups(board);

        for (size_t i = 0; i < groups->size; i++) {
            Group* g = *(Group**)list_get(groups, i);
            if (g && group_is_captured(g)) {
                anyCaptured = true;
                break;
            }
        }

        if (!anyCaptured) {
            tile_set_group(t, NULL);
            group_free(newGroup);
            return MOVE_SUICIDE;
        }
    }

	// check ko
    if (check_ko(board)) {
        List* groups = board_get_groups(board);

        for(size_t i = 0; i < groups->size; i++) {
            Group* g = *(Group**)list_get(groups, i);
            if (g && group_is_captured(g)) {
                group_mark_for_capture(g, false);
            }
        }

        tile_set_group(t, NULL);
        group_free(newGroup);
		return MOVE_KO;
    }

    // update groups and indicate success
    update_board_groups(board, newGroup, pt);
	return MOVE_OK;
}

void board_init_groups(Board* b) {
	list_free(b->groups, group_free_elem);
	b->groups = list_init(sizeof(Group*), 4, NULL);
}

void board_remove_group(Board* b, Group* g) {
    list_remove(b->groups, g);
}

List* board_get_groups(Board* b) {
    printf("groups=%p\nsize=%zu\ncapacity=%zu\n", b->groups, b->groups->size, b->groups->capacity);
    return b->groups;
}
