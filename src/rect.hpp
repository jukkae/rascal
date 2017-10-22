#ifndef RECT_HPP
#define RECT_HPP

#include "point.hpp"

/* Axis-aligned rectangle with integer coordinates */
struct Rect {
	Point topLeft;
	Point bottomRight;

	Rect(Point topLeft, Point bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {;}
	// TODO coordinate modes: tl + br, tl + dimensions, possibly center point + dimensions
	// TODO checks that coordinates are in correct order (or just force them)
	// TODO getters for raw coordinates + dimensions
	Rect(int x0, int y0, int x1, int y1) : topLeft(Point(x0, y0)), bottomRight(Point(x1, y1)) {;}
};

#endif /* RECT_HPP */
