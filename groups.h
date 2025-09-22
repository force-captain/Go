#ifndef GROUPS_H
#define GROUPS_H

#include <stdbool.h>

typedef struct Group Group;
typedef struct Board Board;
typedef struct Point Point;
typedef enum Colour Colour;

Group* group_init(Point pt, Colour colour);
void group_free(Group* g);
void calculate_board_groups(Board* board);

int group_get_liberty_count(Group* g);
int group_get_size(Group* g);

void group_mark_for_capture(Group* g, bool is_captured);

bool group_is_captured(Group* g);

#endif

