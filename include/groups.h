#ifndef GROUPS_H
#define GROUPS_H

#include <stdbool.h>
#include "colour.h"
#include "point.h"

typedef struct Group Group;
typedef struct Board Board;

Group* group_init(Point pt, Colour colour);
void group_free(Group* g);
void group_free_elem(void* elem);

void calculate_board_groups(Board* board, Group* newGroup, Point pt);

int group_get_liberty_count(Group* g);
int group_get_size(Group* g);
void group_remove_liberty(Group* g, Point* pt);
Colour group_get_colour(Group* g);

void update_board_groups(Board* board, Group* newGroup, Point pt);
void group_update_liberties(Board* board, Group* group);

void group_mark_for_capture(Group* g, bool is_captured);

bool group_is_captured(Group* g);

#endif

