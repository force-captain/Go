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

void group_mark_for_capture(Group* g, bool is_captured) {
	g->captured = is_captured;
}

bool group_is_captured(Group* g) {
	return g->captured;
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


void update_board_groups(Board* board, Group* newGroup, Point pt) {
    List* groups = board_get_groups(board);

    // clear captured groups
    for (size_t i = 0; i < groups->size; i++) {
        Group* g = *(Group**)list_get(groups, i);
        
        if (!g) continue;
        if (!g->captured) continue;

        List* points = g->points;
        if (!points) continue;

        for (size_t j = 0; j < points->size; j++) {
            Point p = *(Point*)list_get(points, j);
            tile_set_group(board_get_tile(board, p), NULL);
        }
    }

    Point neighbours[4];
    board_get_neighbours(neighbours, pt);
    Group* to_merge[4];
    int safe_count = 0;
    
    for(size_t i = 0; i < 4; i++) {
        Point n = neighbours[i];
        if (!point_in_bounds(board_get_size(board), n)) continue;

        Group* g = tile_get_group(board_get_tile(board, n));
        if (!g || g == newGroup) continue;
        if (group_get_colour(g) != group_get_colour(newGroup)) {
            list_remove(g->liberties, &n);
            continue;
        }

        int duplicate = 0;
        for(int j = 0; j < safe_count; j++) {
            if (to_merge[j] == g) {
                duplicate = 1;
                break;
            }
        }

        if (duplicate == 0) {
            to_merge[safe_count++] = g;
        }

    }

    for(int i = 0; i < safe_count; i++) {
        merge_groups(board, newGroup, to_merge[i]);
    }

}
