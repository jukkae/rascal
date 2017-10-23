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
	Rect(int x0, int y0, int x1, int y1);

	// naming conventions & constness?
	int x0() { return topLeft.x; }
	int x1() { return bottomRight.x; }
	int y0() { return topLeft.y; }
	int y1() { return bottomRight.y; }
	int width() { return x1()-x0() + 1; }
	int height() { return y1()-y0() + 1; }
};

#endif /* RECT_HPP */
