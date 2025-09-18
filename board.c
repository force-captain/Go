#include <stdio.h>
#include "list.h"
#include "board.h"
#include "states.h"

Board game_board;

void init_board(int size) {
	game_board.size = size;
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			game_board.data[i][j] = NONE;
		}
	}
}

int is_valid_point(Point pt) {
	int size = game_board.size;
	if (pt.x >= size || pt.x < 0 || pt.y >= size || pt.y < 0)
		return 0;
	else
		return 1;
}

void get_neighbours(Point* neighbours, Point pt) {
	neighbours[0] = (Point){ pt.x - 1, pt.y };
	neighbours[1] = (Point){ pt.x + 1, pt.y };
	neighbours[2] = (Point){ pt.x, pt.y - 1 };
	neighbours[3] = (Point){ pt.x, pt.y + 1 };
}

// 0 = Success, 1 = Failure
int place_tile(Tile tile, Point pt) {
	if (is_valid_point(pt) == 0)
		return 1;
	Point neighbours[4];
	get_neighbours(neighbours, pt);
	
	for(int i = 0; i < 4; i++) {
		
	}

	Tile prev = game_board.data[pt.x][pt.y];
	game_board.data[pt.x][pt.y] = tile;
	return 0;
}
