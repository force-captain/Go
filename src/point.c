#include "point.h"

#include <stdbool.h>

bool point_in_bounds(size_t size, Point pt) {
    return pt.x < size && pt.y < size;
}
