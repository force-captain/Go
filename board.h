#ifndef BOARD_H
#define BOARD_H

#include <stddef.h>
#include <stdint.h>

#define MAX_SIZE 19

typedef struct List List;

typedef enum {
	NONE,
	BLACK,
	WHITE
} Tile;

typedef struct Group {
	List* points;
	List* liberties;
	Tile type;
} Group;

typedef struct {
	uint8_t x;
	uint8_t y;
} Point;

typedef struct Board {
	int size;
	Tile data[MAX_SIZE][MAX_SIZE];
} Board;

extern Board game_board;

void init_board(int size);

int is_valid_point(Point pt);

void get_neighbours(Point *neighbours, Point pt);

int place_tile(Tile tile, Point pt);

Group* get_group(Point pt) {

}

#endif
