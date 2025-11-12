#include "point.h"

bool point_in_bounds(size_t size, Point pt) {
    return pt.x < size && pt.y < size;
}

int point_cmp(const void* a, const void* b) {
    Point* apt = (Point*) a;
    Point* bpt = (Point*) b;
    if (apt->x == bpt->x && apt->y == bpt->y)
        return 0;
    else
        return 1;
}
