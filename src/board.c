#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "util/list.h"
#include "groups.h"
#include "board.h"
#include "states.h"

#define YELB "\033[48;5;180m"
#define BLK  "\e[0;30m"
#define WHT  "\e[0;37m"
#define NRM  "\e[0;39m"
#define NRMB "\e[0;49m"

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

// 0 = Success, 1+ = Failure
int board_place_tile(Board* board, Colour colour, Point pt) {
	if (point_in_bounds(board->size, pt) == 0 || board_get_colour(board, pt) != NONE) {
		return MOVE_INVALID; // Invalid move
	}

	List* captured_groups = list_init(sizeof(Group*), 2, NULL);

	Point neighbours[4];
	board_get_neighbours(neighbours, pt);
	for(uint8_t i = 0; i < 4; i++) {
        // Loop over the placed tile's neighbours.
		Point n = neighbours[i];
		if (!point_in_bounds(board->size, n)) {
			continue;
		}
		Tile* t = board_get_tile(board, n);
		if (t->colour != NONE) {
			Group* g = t->group;
			if (t->colour != colour) {
				if (group_get_liberty_count(g) == 1) {
					group_mark_for_capture(g, true);	
					list_append(captured_groups, g);
				}
			}
			else {
				if (group_get_liberty_count(g) == 1) {
					list_free(captured_groups, NULL);
					return MOVE_SUICIDE; // Suicide rule
				}
			}
		}
	}
	int result = board_set_tile(board, colour, pt);
	// Ko check
	if (check_ko(board)) {
		for (size_t i = 0; i < captured_groups->size; i++) {
			Group* g = *(Group**)list_get(captured_groups, i);
			group_mark_for_capture(g, false);
		}
		list_free(captured_groups, NULL);
		board_set_tile(board, NONE, pt);
		return MOVE_KO; // Ko/Superko rule
	}
	list_free(captured_groups, NULL);
    Group* newGroup = group_init(pt, colour);
    update_board_groups(board, newGroup, pt);
	
	return result;
}

void board_init_groups(Board* b) {
	list_free(b->groups, group_free_elem);
	b->groups = list_init(sizeof(Group*), 4, NULL);
}

void board_remove_group(Board* b, Group* g) {
    list_remove(b->groups, g);
}

List* board_get_groups(Board* b) {
    return b->groups;
}

char* board_get_image(Board* b, int term_col) {
    size_t cell_len = 4 + strlen(BLK) + strlen(NRM);
    size_t max_row_len = b->size * cell_len + 3;
    size_t rows = (b->size * 2);
    int padding = (term_col - rows + 1) / 2;
    if (padding < 0) padding = 0;
    char* result = malloc(2 * rows * strlen(YELB) + max_row_len * rows + 1);
    char* p = result;
    if (!result) return NULL;
    result[0] = '\0';

    p += sprintf(p, "%*s", padding, " ");

    p += sprintf(p, "   ");

    for(uint8_t c = 0; c < b->size; c++) {
        p += sprintf(p, " %c  ", 'A' + c);
    }
    p += sprintf(p, "\n");

    for(uint8_t r = 1; r < rows; r++) {
        p += sprintf(p, "%*s", padding, " ");
        if (r % 2 == 0) {
            p += sprintf(p, "   "YELB);
            for (int i = 0; i < b->size - 1; i++) {
                p += sprintf(p, "---+");
            }
            p += sprintf(p, "---");
        } else {
            p += sprintf(p, "%2d "YELB, (r+1) / 2);
            for(uint8_t c = 0; c < b->size; c++) {
                Colour clr = b->data[r/2][c].colour;
                if (clr == NONE) {
                    p += sprintf(p, "   ");
                } else {
                    if (clr == BLACK) p += sprintf(p, " "BLK"O"NRM" ");
                    else p += sprintf(p, " "WHT"O"NRM" ");
                }
                if (c < b->size - 1) {
                    p += sprintf(p, "|");
                }
            }
        }
        p += sprintf(p, NRMB"\n");
    }
    p += sprintf(p, NRMB);
    return result;
}

/*
 * YELB BLK O NRM | BLK O NRM | BLK O NRM | BLK O NRM
 * ---+---+---+
 * BLK O NRM |
 * 
 * NRMB
 * 
*/
