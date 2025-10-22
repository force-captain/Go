#ifndef BOARD_H
#define BOARD_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "colour.h"
#include "point.h"

#define MAX_SIZE 19

// Forward declaration from util/list
typedef struct List List;
// Opaque structs
typedef struct Board Board;
typedef struct Group Group;
typedef struct Tile Tile;

typedef enum {
	MOVE_OK,
	MOVE_INVALID,
	MOVE_SUICIDE,
	MOVE_KO
} MoveResult;

// Interface
Colour board_get_colour(Board* b, Point pt);
int board_set_tile(Board* b, Colour colour, Point pt);
Tile* board_get_tile(Board* b, Point pt);
Board* board_init(uint8_t size);
void board_free(Board* board);
List* board_get_groups(Board* b);
void board_init_groups(Board* b);
int board_add_group(Board* b, Group* g);
uint8_t board_get_size(Board* b);

void board_append_group(Board* b, Group* g);
void board_remove_group(Board* b, Group* g);

Colour tile_get_colour(Tile* t);
Group* tile_get_group(Tile* t);

void tile_set_group(Tile* t, Group* g);

bool point_is_in_bounds(uint8_t size, Point pt);

void board_get_neighbours(Point out[4], Point pt);

int board_place_tile(Board* board, Colour tile, Point pt);

Group* board_get_group(Point pt);

#endif
