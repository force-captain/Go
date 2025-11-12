#include "groups.h"
#include "board.h"
#include "point.h"
#include "util/list.h"
#include <stdio.h>

typedef struct Group {
	List* points;
	List* liberties;
	Colour colour;
	bool captured;
} Group;


Group* group_init(Point pt, Colour colour) {
	Group* g = malloc(sizeof(Group));
	g->points = list_init(sizeof(Point), 4, point_cmp);
	g->liberties = list_init(sizeof(Point), 4, point_cmp);
	g->colour = colour;
	g->captured = false;
	list_append(g->points, &pt);
	return g;
}

void group_free(Group* g) {
	if (g->points) list_free(g->points, NULL);
	if (g->liberties) list_free(g->liberties, NULL);
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

void group_remove_liberty(Group* g, Point* pt) {
    list_remove(g->liberties, pt);
}

void group_mark_for_capture(Group* g, bool is_captured) {
	g->captured = is_captured;
}

bool group_is_captured(Group* g) {
	return g->captured;
}

void group_update_liberties(Board* b, Group* g) {
    list_clear(g->liberties);
    for(size_t i = 0; i < g->points->size; i++) {
        Point pt = *(Point*)list_get(g->points, i);
        Point neighbours[4];
        board_get_neighbours(neighbours, pt);
        for(int j = 0; j < 4; j++) {
            Point n = neighbours[j];
            if (!point_in_bounds(board_get_size(b), n)) continue;

            Tile* t = board_get_tile(b, n);
            if (tile_get_colour(t) == NONE) {
                if (list_contains(g->liberties, &n) == -1) {
                    list_append(g->liberties, &n);
                }
            }
        }
    }
}

void group_update_neighbours(Board* b, Point pt) {
    Point neighbours[4];
    board_get_neighbours(neighbours, pt);

    for (int i = 0; i < 4; i++) {
        Point n = neighbours[i];
        if (!point_in_bounds(board_get_size(b), n)) continue;

        Tile* t = board_get_tile(b, n);
        if (tile_get_group(t))
            group_update_liberties(b, tile_get_group(t));
    }
}

void merge_groups(Board* b, Group* main, Group* extra) {
    if (!extra || extra == main) return;

    // merge lists
    list_merge(main->liberties, extra->liberties);
    list_merge(main->points, extra->points);

    // update tiles
    for (size_t i = 0; i < extra->points->size; i++) {
        Point pt = *(Point*)list_get(extra->points, i);
        tile_set_group(board_get_tile(b, pt), main);
    }

    // deduplicate liberties
    for (ssize_t i = (ssize_t)main->liberties->size-1; i >= 0; i--) {
        Point pt = *(Point*)list_get(main->liberties, i);
        if (list_contains(main->points, &pt) != -1) {
            list_remove(main->liberties, &pt);
        }
    }

    // detach
    extra->points = NULL;
    extra->liberties = NULL;

    // delete group
    board_remove_group(b, extra);
    group_free(extra);
}

void clear_group(Board* board, Group* captured) {
    List* points = captured->points;
    if (!points) return;

    List* neighbour_groups = list_init(sizeof(Group*), 8, NULL);
    
    for (size_t i = 0; i < points->size; i++) {
        Point p = *(Point*)list_get(points, i);
        tile_set_group(board_get_tile(board, p), NULL);

        Point neighbours[4];
        board_get_neighbours(neighbours, p);
        for (int j = 0; j < 4; j++) {
            Point n = neighbours[j];
            if (!point_in_bounds(board_get_size(board), n)) continue;

            Group* adj = tile_get_group(board_get_tile(board, n));
            if (!adj) continue;
            if (adj == captured) continue;
            if (list_contains(neighbour_groups, &adj) != -1) continue;

            list_append(neighbour_groups, adj);
        }
    }

    for(size_t i = 0; i < neighbour_groups->size; i++) {
        group_update_liberties(board, list_get(neighbour_groups, i));
    }

    list_free(neighbour_groups, NULL);

    List* groups = board_get_groups(board);
    list_remove(groups, &captured);
    group_free(captured);
}


void update_board_groups(Board* board, Group* newGroup, Point pt) {
    List* groups = board_get_groups(board);

    // clear captured groups
    for(size_t i = 0; i < groups->size; i++) {
        Group* g = *(Group**)list_get(groups, i);
        if (g && g->captured) {
            clear_group(board, g);
        }
    }

    Point neighbours[4];
    board_get_neighbours(neighbours, pt);
    
    // merge neighbours
    for(size_t i = 0; i < 4; i++) {
        Point n = neighbours[i];
        if (!point_in_bounds(board_get_size(board), n)) continue;

        Group* g = tile_get_group(board_get_tile(board, n));
        if (!g || g == newGroup) continue;
        if (group_get_colour(g) != group_get_colour(newGroup)) continue;

        merge_groups(board, newGroup, g);
    }

    // update liberties
    group_update_liberties(board, newGroup);
    group_update_neighbours(board, pt);
}
