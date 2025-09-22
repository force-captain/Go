#include "groups.h"
#include "board.h"
#include "list.h"

typedef struct Group {
	List* points;
	List* liberties;
	Colour colour;
	bool captured;
} Group;

Group* group_init(Point pt, Colour colour) {
	Group* g = malloc(sizeof(Group));
	g->points = list_init(sizeof(Point), 4);
	g->liberties = list_init(sizeof(Point), 4);
	g->colour = colour;
	g->captured = false;
	list_append(g->points, pt);
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

void group_mark_for_capture(Group* g, bool is_captured) {
	g->captured = is_captured;
}

bool group_is_captured(Group* g) {
	return g->captured;
}

void calculate_board_groups(Board* board) {
	List* groups = board_get_groups(board);
	for (int i = 0; i < groups->size; i++) {
		Group* g = *(Group**)list_get(groups, i);
		if (g->captured) {
			List* points = g->points;
			for (int j = 0; j < points->size; j++) {
				Point p = *(Point*)list_get(points, j);
				tile_set_group(board_get_tile(board, p), NULL);	
			}
		}
	}

	list_free(groups, group_free_elem);
	board->groups = list_init(sizeof(Group*), 4);

	uint8_t size = board->size;
	bool visited[size][size] = {0};
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (!visited[i][j]) {
				Point pt = {.x = i, .y = j};
				Colour colour = board_get_colour(board, pt);
				if (colour == NONE) {
					visited[i][j] = true;
					continue;
				} else {
					bool liberties_visited[size][size] = {0};
					Group* g = group_init(pt, colour);
					visited[i][j] = true;
					Tile* t = board_get_tile(board, pt);
					tile_set_group(t, g);
					explore_group(size, visited, liberties_visited, board, g, pt);
					board_append_group(board, g);
				}
			}
		}
	}
}

static void explore_group(int size, bool visited[size][size], bool liberties_visited[size][size], Board* board, Group* group, Point pt) {
	Point neighbours[4];
	get_neighbours(neighbours, pt);
	for (int i = 0; i < 4; i++) {
		Point newpt = neighbours[i];
		Tile* newt = board_get_tile(board, newpt);
		if (is_valid_point(newpt) && visited[newpt.x][newpt.y] == false) {
			Colour colour = board_get_colour(board, newpt);
			if (colour == NONE) {
				if (liberties_visited[newpt.x][newpt.y] == false) {
					list_append(group->liberties, newpt);
					liberties_visited[newpt.x][newpt.y] = true;
				}
			} else if (colour == group->colour) {
				visited[newpt.x][newpt.y] = true;
				list_append(group->points, newpt);
				tile_set_group(newt, group);
				explore_group(size, visited, liberties_visited, board, group, newpt);
			}
		}
	}
}
