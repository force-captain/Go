#ifndef POINT_H
#define POINT_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t x;
	uint8_t y;
} Point;

bool point_in_bounds(size_t size, Point pt);

#endif
