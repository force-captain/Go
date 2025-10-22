#include "groups.h"
#include "board.h"
#include "util/list.h"

typedef struct Group {
	List* points;
	List* liberties;
	Colour colour;
	bool captured;
} Group;


Group* group_init(Point pt, Colour colour) {
	Group* g = malloc(sizeof(Group));
	g->points = list_init(sizeof(Point), 4, NULL);
	g->liberties = list_init(sizeof(Point), 4, NULL);
	g->colour = colour;
	g->captured = false;
	list_append(g->points, &pt);
	return g;
}

void group_free(Group* g) {
	list_free(g->points, NULL);
	list_free(g->liberties, NULL);
	free(g);
}

void group_free_elem(void* elem) {
	Group* g = *(Group**)elem;
	group_free(g);
}

int group_get_liberty_count(Group* g) {
	return g->liberties->size;
}

int group_get_size(Group* g) {
	return g->points->size;
}

Colour group_get_colour(Group* g) {
    return g->colour;
}

void group_mark_for_capture(Group* g, bool is_captured) {
	g->captured = is_captured;
}

bool group_is_captured(Group* g) {
	return g->captured;
}

void merge_groups(Board* b, Group* main, Group* extra) {
    list_merge(main->liberties, extra->liberties);
    list_merge(main->points, extra->points);
    for (ssize_t i = (ssize_t)main->liberties->size-1; i >= 0; i--) {
        Point pt = *(Point*)list_get(main->liberties, i);
        if (list_contains(main->points, &pt) != -1) {
            list_remove(main->liberties, &pt);
        }
    }

    board_remove_group(b, extra);
    group_free(extra);
}


void update_board_groups(Board* board, Group* newGroup, Point pt) {
    List* groups = board_get_groups(board);
    for (size_t i = 0; i < groups->size; i++) {
        Group* g = *(Group**)list_get(groups, i);
        if (g->captured) {
            List* points = g->points;
            for (size_t j = 0; j < points->size; j++) {
                Point p = *(Point*)list_get(points, j);
                tile_set_group(board_get_tile(board, p), NULL);
            }
        }
    }

    Point neighbours[4];
    board_get_neighbours(neighbours, pt);
    
    for(size_t i = 0; i < 4; i++) {
        Point pt = neighbours[i];
        if (!point_in_bounds(board_get_size(board), pt)) continue;
        Group* g2 = tile_get_group(board_get_tile(board, pt));
        if (g2 != NULL && group_get_colour(g2) == group_get_colour(newGroup)) {
            merge_groups(board, newGroup, g2);
        }
    }
}
